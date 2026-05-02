function sumOf(items){
    let sum = 0
    for(let i in items){
        sum += i
    }
    return sum
}

function avg(items) = sumOf(items) / 2

let array = [1,2,9]

println("Average of {array} is {avg(array)}")