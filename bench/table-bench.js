// bench/table-bench.js: the State Table for bench pairs, in code.
//
// bench.html loads this file instead of table.js, so a bench pair's rows live
// here. Same format as table.js: signals are the numbers sense-serial.js writes
// (each 0 to 1, named in its SIGNALS), states are checked top to bottom and the
// first true row wins.
//
// Hold is in seconds, matching names-distance.ino. The browser table is only an
// optional mirror; the Nano runs these names and responses without a computer.

window.TABLE = {
  signals: ['near', 'present'],

  states: [
    { name: 'near',    when: s => s.near > 0.5, hold: 1.0 },
    { name: 'far',     when: s => s.present > 0.5 && s.near <= 0.5, hold: 1.0 },
    { name: 'nobody',  when: s => s.present <= 0.5, hold: 5.0 },
  ],
};
