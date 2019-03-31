open Pervasives;

type user = {
  email: string,
  id: float,
  name: string,
};

let decodeUsers =
  Js.Json.decodeArray
  >> Belt.Option.getWithDefault(_, [||])
  >> Belt.Array.keepMap(
       _,
       Js.Json.decodeObject
       >> Belt.Option.flatMap(_, x =>
            switch (
              Js.Dict.get(x, "email")
              |> Belt.Option.flatMap(_, Js.Json.decodeString),
              Js.Dict.get(x, "id")
              |> Belt.Option.flatMap(_, Js.Json.decodeNumber),
              Js.Dict.get(x, "name")
              |> Belt.Option.flatMap(_, Js.Json.decodeString),
            ) {
            | (Some(email), Some(id), Some(name)) =>
              Some({email, id, name})
            | _ => None
            }
          ),
     );

type state = {
  counter: int,
  users: option(array(user)),
};

[@bs.deriving accessors]
type action =
  | Fetch
  | FetchOver(result(array(user), string))
  | Increment
  | Decrement;

let effects = _state =>
  fun
  | Fetch =>
    Middlewares.Effect.attempt(
      fetchOver,
      Middlewares.Effect.taskFromPromise(
        Fetch.fetch("http://jsonplaceholder.typicode.com/users")
        |> Js.Promise.then_(Fetch.Response.json)
        |> Js.Promise.then_(decodeUsers >> Js.Promise.resolve),
        ~mapError=const("An error occured"),
      ),
    )
  | Increment =>
    Middlewares.Effect.perform(
      const(increment),
      Middlewares.Effect.sleep(1000),
    )
  | Decrement
  | FetchOver(_) => Middlewares.Effect.none;

let reducer = state =>
  fun
  | Fetch => state
  | FetchOver(Ok(users)) => {...state, users: Some(users)}
  | FetchOver(Error(_)) => {...state, users: Some([||])}
  | Increment => {...state, counter: state.counter + 1}
  | Decrement => {...state, counter: state.counter - 1};

let initialState = {counter: 0, users: None};

[@react.component]
let make = () => {
  let (state, dispatch) = reducer->React.useReducer(initialState);

  let dispatchWithEffect =
    effects(state)->Middlewares.Effect.apply(dispatch);

  <div>
    <div> {("Counter: " ++ state.counter->string_of_int)->React.string} </div>
    <div>
      <button onClick={_ => dispatchWithEffect(Increment)}>
        "+"->React.string
      </button>
    </div>
    <div>
      <button onClick={_ => dispatchWithEffect(Decrement)}>
        "-"->React.string
      </button>
    </div>
    <div>
      <button onClick={_ => dispatchWithEffect(Fetch)}>
        "Fetch"->React.string
      </button>
    </div>
    <div>
      {switch (state.users) {
       | None => <div> "No users fetch yet"->React.string </div>
       | Some([||]) => <div> "No users found"->React.string </div>
       | Some(users) =>
         <div>
           {{users
             |> Belt.Array.map(_, ({email, id, name}) =>
                  <div key={id->Js.Float.toString}>
                    {(name ++ ": " ++ email)->React.string}
                  </div>
                )}
            ->React.array}
         </div>
       }}
    </div>
  </div>;
};