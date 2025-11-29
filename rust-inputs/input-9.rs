fn main() -> i64 {
    let mut a: i64 = 3;
    let mut b: i64 = 4;
    let mut d: i64 = 0;

    if (a < b) {
        d = a + b;
        println!("{}", d);
    } else {
        println!("{}", a);
    }
    return 0;
}
