static mut X: i32 = 0;

fn suma(a: i32, b: i32) -> i32 {
    let mut y: i32 = 0;
    y = a + b;
    return y;
}

fn main() -> i32 {
    println!("{}", suma(2, 3));
    return 0;
}
