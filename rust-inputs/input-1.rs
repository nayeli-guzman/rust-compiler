static mut X: i64 = 0;

fn suma(a: i64, b: i64) -> i64 {
    let mut y: i64 = 0;
    y = a + b;
    return y;
}

fn main() -> i64 {
    println!("{}", suma(2, 3));
    return 0;
}
