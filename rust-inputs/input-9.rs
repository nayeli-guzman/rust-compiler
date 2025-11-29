fn main() -> i32 {
    let mut a: i32 = 3;
    let mut b: i32 = 4;
    let mut d: i32 = 0;

    if (a < b) {
        d = a + b;
        println!("{}", d);
    } else {
        println!("{}", a);
    }
    return 0;
}
