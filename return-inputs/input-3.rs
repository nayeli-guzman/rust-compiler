fn hola() -> [i64; 2] {
    let mut a: i64 = 10;
    let mut b: i64 = 20;

    let mut tmp: i64 = a;
    a = b;
    b = tmp;

    return([a, b]);
}

fn main() {
    let arr: [i64; 2] = hola();
    println!("{}", arr[0]);
    println!("{}", arr[1]);
}
