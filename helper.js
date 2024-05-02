function escriuAccions() { // After the HTML content loads
   

    
    for (let i = 0; i < 10; i++) {
        let data = new Date();
        var dateOffset = (24*60*60*1000) * i;
        
        data.setTime(data.getTime() - dateOffset);
        var dia = formatejaData(data)
        
        escriu_accio(dia);
      } 
    
};

function formatejaData(data) {
   
    const offset = data.getTimezoneOffset()
   
    data = new Date(data.getTime() - (offset*60*1000));
    var dataForm = data.toISOString().split('T')[0]

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


