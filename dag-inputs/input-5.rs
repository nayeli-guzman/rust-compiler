fn main() {
    let mut a: i64 = 1;
    let mut b: i64 = 2;
    let mut c: i64 = 3;

    let mut s1: i64 = a + b;
    let mut s2: i64 = a + b;

    let mut s3: i64 = 0;

    c = c + 1;          

    s3 = a + b;

    println!("{}", s1);
    println!("{}", s2);
    println!("{}", s3);
}
