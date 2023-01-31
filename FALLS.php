<?php
    // define PDD - tell about database file

    $pdo = new PDO('sqlite:DB_HealthRemoteMonitorSystem.db');


    //write SQL
    $statement = $pdo->query("SELECT * FROM FALLS");

    //run SQL
    $rows = $statement->fetchAll(PDO::FETCH_ASSOC);
    
    $stackBPM = array();
    $stackTSBPM = array();

    //show it on the screen
    //print_r($rows);
    
    print "<h2>History of Fall Emergencies:</h2>";


    print '<table style="width:100%">';

    print "<table border=1 cellspacing=1 cellpadding=1>";
    print "<tr>";
    print "<th style =";
    print '"background-color:#b5b5f7">';
    print "Fall Number</th>";
    print "<th style =";
    print '"background-color:#b5b5f7">';
    print "Date</th>";
    print "</tr>";


    foreach($rows as $row)
    {

       print "<tr>" ;
       print "<td>".$row['FALL_ID']."</td>";
       print "<td>".$row['TIMESTAMP']."</td>";
       print "</tr>";
  /*      
    print "<td>".$row['FALL_ID']."</td>";
    array_push($stackBPM,$row['FALL_ID']);
   // print "<td>".$stackBPM[0]."</td>";
    array_push($stackTSBPM,$row['TIMESTAMP']);
    print "<td>".$row['TIMESTAMP']."</td>";
    print "<p>";
    */
    }

    print "</table>";

    
print '<div class="wrapper">';
print '<button onclick="location.href=';
print "'http://localhost:1234/'";
print '" type="button"';
print '  class="button">';
print "         Return to App Menu</button>
         </div>";




?>
<style>
  .wrapper {
    text-align: center;
}
table, th, td {
  border:1px solid black;
}
.button {
    background-color: #000; /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
}
</style>
