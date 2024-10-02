
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


const output = document.getElementById('main_stocks')


// document.getElementById('file').onchange = function() {
function carregaFitxer() {
    var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // Typical action to be performed when the document is ready:
          separaLinies(xhttp.responseText) //callback function
          escrDates(xhttp.responseText) //callback function
        }
    };
    xhttp.open("GET", "/output/pertiles.txt", true);
    xhttp.send();
};

function escrDates(text){
  var lines = text.split('\n');    // lines is an array of strings
  descrip = document.getElementById("descr")

  linia = lines[0].split(" ")
  linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
  data1 = linia[2]

  linia = lines[1].split(" ")
  linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
  data2 = linia[2]

  descrip.innerHTML = descrip.innerHTML +" "+ data1 + " --> " + data2;
}

function separaLinies(text) {
  var lines = text.split('\n');    // lines is an array of strings
  taula = document.getElementById("taula")

  // Loop through all lines
  for (var j = 0; j < lines.length; j++) {
    linia = lines[j].split(' ')
    linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
    if(!isNaN(linia[0])) {
      
      let row = taula.insertRow(-1); // Create an empty <tr> element and add it to the last position of the table:

      // Insert new cells (<td> elements) at the 1st and 2nd position of the "new" <tr> element:
      var cell1 = row.insertCell(0);
      var cell2 = row.insertCell(1);
      var cell3 = row.insertCell(2);
      var cell4 = row.insertCell(3);

      // Add some text to the new cells:
      cell1.innerHTML = linia[0];
      cell2.innerHTML = linia[1];
      cell3.innerHTML = linia[2];
      cell4.innerHTML = linia[3];
    }
  }
}