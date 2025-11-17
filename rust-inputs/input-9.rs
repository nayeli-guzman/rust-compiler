fn main() -> i32 {
    let mut a: i32;
    let mut b: i32;
    let mut c: i32;
    a = 3;
    b = 4;
    if a < b {
        let mut d: i32;
        d = a + b;
        println!("{}", d);
    } else {
        println!("{}", a);
    }
    return 0;
}
