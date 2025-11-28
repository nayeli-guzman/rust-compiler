struct Punto {
    x: i64,
    y: i64,
}

fn main() {

    let mut nums: [i64; 5] = [1, 2, 3, 4, 5];

    println!("{}", nums[0]); 
    println!("{}", nums[3]); 

    nums[1] = 100;
    println!("{}", nums[1]); 

    return(0)
}
