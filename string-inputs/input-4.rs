fn main() {
    let mut palabras: [String; 3] = [
        "uno",
        "dos",
        "tres"
    ];
    let i: i64 = 1;


    println!("{}", "---- Programa 4 ----");
    println!("{}", palabras[0]);
    println!("{}", palabras[1]);
    println!("{}", palabras[2]);

    palabras[i] = "DOS EN MAYUS";

    println!("{}", palabras[i]);

    return(0)
}
