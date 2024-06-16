
function ompleLlistes() {
    escriuAccions("output/stocks_slope_percent_", "llista","csv")
    escriuAccions("output/stocks_sorted_stats_", "llista2","txt")
}

function escriuAccions(filenameSufix, elementId, ext) { // After the HTML content loads

    for (let i = 0; i < 10; i++) {
        let data = new Date();
        var dateOffset = (24*60*60*1000) * i;
        
        data.setTime(data.getTime() - dateOffset);
        var dia = formatejaData(data)
        
        dies = ["Diumenge", "Dilluns", "Dimarts", "Dimecres", "Dijous", "Divendres", "Dissabte"];
        // escriu els dies excepte per caps de setmana, ja que no hi ha canvis de borsa gairebé, els findes
        nom = dia + " " + dies[data.getDay()]
        filename = filenameSufix+dia + "."+ext
        if(data.getDay() != 6 && data.getDay() != 0) escriu_link(nom, filename, elementId);
      } 
    
};

function formatejaData(data) {
   
    const offset = data.getTimezoneOffset()
   
    data = new Date(data.getTime() - (offset*60*1000));
    
    var dataForm = data.toISOString().split('T')[0]

    

    return dataForm;
}

function escriu_link(nom,filename, elementId) { // After the HTML content loads
    let list = document.getElementById(elementId);
    var inner = "";

    inner += "<a href='"+filename +"'> accions dia " + nom + " </a>";
    
    let li = document.createElement('li');
    li.innerHTML =inner;
    list.appendChild(li); // Appends HTML to an element
};


