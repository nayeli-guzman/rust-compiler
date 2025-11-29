fn main() {
    let mut a: [i64; 5] = [1, 2, 3, 4, 5];

    println!("{}", a[0]);
    println!("{}", a[3]);

    a[1] = 100;
    println!("{}", a[1]);

    return(0);
}
