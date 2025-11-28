fn hola() -> [i64; 2] {
    let mut x: i64 = 10;
    let mut y: i64 = 20;
    return([7, 9]);
}

fn main() {
    let arr: [i64; 2] = hola();
    println!("{}", arr[0]);
    println!("{}", arr[1]);
}
