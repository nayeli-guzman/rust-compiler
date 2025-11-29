fn suma3(x: i64, y: i64, z: i64) -> i64 {
    let a: i64 = 1;
    let b: i64 = 2;
    let c: i64 = 3;

    let r: i64 = x + y;
    let s: i64 = r + z + 0;
    let t: i64 = s + a + b + c;

    return t;
}

fn main() {
    let res: i64 = suma3(5, 6, 7);
    println!("{}", res);
    return(0);
}
