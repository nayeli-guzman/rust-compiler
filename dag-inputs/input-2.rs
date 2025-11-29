fn main() {
    let mut x: i64 = 5;
    let mut y: i64 = 7;

    let mut a: i64 = x + y;
    let mut b: i64 = x + y;

    let mut c: i64 = 0;

    x = x + 1;    

    c = x + y;


    println!("{}", a);
    println!("{}", b);
    println!("{}", c);
}
