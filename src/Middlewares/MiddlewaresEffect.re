open Pervasives;
open Js.Promise;

type effect('action) =
  | Effect(Js.Promise.t('action))
  | None;

type trusted =
  | Trusted;

type task('ok, 'error) = t(result('ok, 'error));

let taskFromPromise = (promise, ~mapError) =>
  promise |> then_(ok >> resolve) |> catch(mapError >> error >> resolve);

let sleep = ms =>
  taskFromPromise(
    make((~resolve, ~reject as _) =>
      ignore(ms |> Js.Global.setTimeout(() => resolve(. unit)))
    ),
    ~mapError=const(Trusted),
  );

let none = None;

let perform = (actionCreator, task) =>
  Effect(task |> then_(getExn >> actionCreator >> resolve));

let attempt = (actionCreator, task) =>
  Effect(task |> then_(actionCreator >> resolve));

let apply = (effects, dispatch, action) => {
  switch (effects(action)) {
  | None => ()
  | Effect(action) => ignore(action |> then_(dispatch >> resolve))
  };

  dispatch(action);
};