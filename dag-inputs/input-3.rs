fn main() {
    let mut x: i64 = 2;
    let mut y: i64 = 3;
    let mut z: i64 = 4;

    let mut s1: i64 = (x + y) * z;
    let mut s2: i64 = (x + y) * z;
    let mut s3: i64 = (x + y) * z;

    println!("{}", s1);
    println!("{}", s2);
    println!("{}", s3);
}
