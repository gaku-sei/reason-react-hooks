type effect('action);

type trusted;

type task('ok, 'error);

let taskFromPromise:
  (Js.Promise.t('ok), ~mapError: Js.Promise.error => 'error) =>
  task('ok, 'error);

let sleep: int => task(unit, trusted);

let none: effect('action);

let perform: ('ok => 'action, task('ok, trusted)) => effect('action);

let attempt:
  (result('ok, 'error) => 'action, task('ok, 'error)) => effect('action);

let apply: ('action => effect('action), 'action => unit, 'action) => unit;