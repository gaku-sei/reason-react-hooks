let (<<) = (f, g, x) => f(g(x));

let (>>) = (f, g, x) => g(f(x));

let const = (x, _) => x;

let identity = x => x;

let unit = ();

let ok = x => Ok(x);

let error = e => Error(e);

exception GetExnException;

let getExn =
  fun
  | Ok(x) => x
  | Error(_) => raise(GetExnException);