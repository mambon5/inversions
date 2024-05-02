function escriuAccions() { // After the HTML content loads
    var div = document.getElementById("llista");
    var inner = "";

        let yourDate = new Date();
    const offset = yourDate.getTimezoneOffset()
   
    yourDate = new Date(yourDate.getTime() - (offset*60*1000));
    var avui = yourDate.toISOString().split('T')[0]
    var filename = "output/stocks_slope_percent_"+avui + ".csv"

    inner += "<a href='"+filename+"'> accions dia " + avui + " </a>";
    
    let li = document.createElement('li');
    li.innerHTML =inner;
    list.appendChild(li); // Appends HTML to an element
};

// const path = require('path');
// console.log(path.resolve("readMe.txt"));

// var fs = require('fs');
// var files = fs.readdirSync('/output/');

 let list = document.getElementById("llista");
// for (i = 0; i < files.length; ++i) {
//     let li = document.createElement('li');
//     li.innerText = files[i];
//     list.appendChild(li);
// }