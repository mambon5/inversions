function escriuAccions() { // After the HTML content loads
   

    let yourDate = new Date();
    
    let data2 = yourDate;
    data2.setDate(data2.getDate() - 1);

    var avui = formatejaData(yourDate)
    var ahir = formatejaData(data2)
    
    escriu_accio(avui);
    escriu_accio(ahir);
};

function formatejaData(data) {
   
    const offset = data.getTimezoneOffset()
   
    yourDate = new Date(yourDate.getTime() - (offset*60*1000));
    var dataForm = yourDate.toISOString().split('T')[0]

    return dataForm;
}

function escriu_accio(data) { // After the HTML content loads
    let list = document.getElementById("llista");
    var inner = "";

    
    var filename = "output/stocks_slope_percent_"+data + ".csv"

    inner += "<a href='"+filename+"'> accions dia " + data + " </a>";
    
    let li = document.createElement('li');
    li.innerHTML =inner;
    list.appendChild(li); // Appends HTML to an element
};


