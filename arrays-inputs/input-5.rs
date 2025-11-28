struct Punto {
    x: i64,
    y: i64,
}

fn main() {
    let mut nums: [i64; 5] = [1, 2, 3, 4, 5];
    let i: i64 = 2;

    let mut puntos: [Punto; 3] = [
        Punto { x: 10, y: 20 },
        Punto { x: 30, y: 40 },
        Punto { x: 50, y: 60 }
    ];

    println!("{}", nums[0]);
    println!("{}", nums[3]);

    nums[1] = 100;
    println!("{}", nums[1]);

    println!("{}", puntos[0].x); 
    println!("{}", puntos[1].y); 

    puntos[2].x = 999;
    println!("{}", puntos[2].x); 

    println!("{}", nums[i]);

    return(0)

}
