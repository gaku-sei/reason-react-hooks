let (<<): ('b => 'c, 'a => 'b, 'a) => 'c;

let (>>): ('a => 'b, 'b => 'c, 'a) => 'c;

let const: ('a, _) => 'a;

let identity: 'a => 'a;

let unit: unit;

let ok: 'ok => result('ok, 'error);

let error: 'error => result('ok, 'error);

exception GetExnException;

let getExn: result('ok, 'error) => 'ok;