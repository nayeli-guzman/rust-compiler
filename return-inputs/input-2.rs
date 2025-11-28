fn gen() -> [i64; 4] {
    return([1, 2, 3, 4]);
}

fn main() {
    let a: [i64; 4] = gen();
    println!("{}", a[3]);
}
