fn main() {
    let mut x: i64 = 10;
    let mut y: i64 = 1;

    let mut a: i64 = x - y;
    let mut b: i64 = x - y;

    let mut c: i64 = 0;
    let mut t: i64 = 0;


    if (x < y) {
        t= x - y;
        println!("{}", t);
    }

    c = x - y;

    println!("{}", a);
    println!("{}", b);
    println!("{}", c);
}
