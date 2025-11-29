fn build() -> [i64; 2] {
    let mut a: i64 = 10;
    let mut b: i64 = 20;

    let mut sum: i64 = a + b;
    let mut prod: i64 = a * b;

    return([sum, prod]); 
}

fn main() {
    let arr: [i64; 2] = build();
    println!("{}", arr[0]);
    println!("{}", arr[1]);
}
