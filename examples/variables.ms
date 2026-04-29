//system types
let a = null                // null
let b = true                // bool
let c = 'a'                 // char
let d = -5                  // int
let e = 0.1                 // real
let f = "string"            // string
let g = [0,1,2]             // array
let i = { "key": "value" }  // dict

let m = [] // empty array
let n = {} // empty dict

// special types
let o = File("temp.bin")    // file
let p = 0..5                // range (also have function-constructor -> range(from,to,step))
let r = 0..10:2             // range with step
let s = { a,b,c -> a+b-c }  // lambda

let sp = { -> }             // empty lambda (mostly useless thing)