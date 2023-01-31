<?php
    // define PDD - tell about database file

    $pdo = new PDO('sqlite:DB_HealthRemoteMonitorSystem.db');


    //write SQL
    $statement = $pdo->query("SELECT * FROM SPO2");

    //run SQL
    $rows = $statement->fetchAll(PDO::FETCH_ASSOC);
    
    $stackSPO2 = array();
    $stackTSSPO2 = array();

    //show it on the screen
    //print_r($rows);
    foreach($rows as $row)
    {
    //print "<td>".$row['SPO2']."</td>";
    array_push($stackSPO2,$row['SPO2']);
    //print "<td>".$stackSPO2[0]."</td>";
    array_push($stackTSSPO2,$row['TIMESTAMP']);
   // print "<td>".$row['TIMESTAMP']."</td>";
    //print "<p>";
    }
    //print "</table>";

?>

<style>
  .wrapper {
    text-align: center;
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

<!DOCTYPE html>
<html>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js"></script>
<body>

<canvas id="myChart" style="width:100%;max-width:600px"></canvas>

<script>
    var obj = <?php echo json_encode($stackSPO2); ?>;
    var objTS = <?php echo json_encode($stackTSSPO2); ?>;

new Chart("myChart", {
  type: "line",
  data: {
    labels: objTS,
    datasets: [{
      fill: false,
      pointRadius: 1,
      borderColor: "rgba(255,0,0,0.5)",
      data: obj
    }]
  },    
  options: {
    scales: {
        xAxes: [{
            gridLines: {
                color: "rgba(0, 0, 0, 0)",
            }
        }],
        yAxes: [{
          gridLines: {
                color: "rgba(0, 0, 0, 0)",
            },
          ticks: {
            beginAtZero: false,
            fixedStepSize: 1,
            max: 100
             }
            
        }]
    },
    legend: {display: false},
    title: {
      display: true,
      text: "SpO2: Oxigen Saturation",
      fontSize: 16
    }
  }
});

</script>

<div class="wrapper">
<button onclick="location.href='http://localhost:1234/SPO2ALL.php'" type="button"  class="button">
         Consult SPO2 Historic</button>
         </div>

<h2></h2>


<div class="wrapper">
<button onclick="location.href='http://localhost:1234/'" type="button"  class="button">
         Return to App Menu</button>
         </div>

</body>
</html>
