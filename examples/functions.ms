function sumOf(items){
    let sum = 0
    for(let i in items){
        sum += i
    }
    return sum
}

function avg(items){
    return sumOf(items) / 2
}

let array = [1,2,9]

println("Average of {array} is {avg(array)}")
//output:
// Average of [1, 2, 9] is 6