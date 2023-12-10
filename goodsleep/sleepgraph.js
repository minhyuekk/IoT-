// API 시작
var intervalId = null;
window.addEventListener("load", drawChart);

function Start() {
	invokeAPI();
  intervalId = setInterval(invokeAPI, 1000);
}
// API 중지
	
function Stop() {
  if(intervalId != null) {
    clearInterval(intervalId);
    document.getElementById("result").innerHTML="No Data";
  }
}
	
var invokeAPI = function() {
    // 디바이스 조회 URI
    // prod 스테이지 편집기의 맨 위에 있는 "호출 URL/devices"로 대체해야 함
    var API_URI = 'https://zywat57ay6.execute-api.ap-northeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010'; 		        
    $.ajax(API_URI, {
        method: 'GET',
        contentType: "application/json",
	
        success: function (data, status, xhr) {
                var result = JSON.parse(data);
                addChartData1(result.state.reported.pulse);
				addChartData2(result.state.reported.temperature);
				addChartData3(result.state.reported.body_temperature);
				
        },
        error: function(xhr,status,e){
        				console.log("AJAX request failed: " + status + ", " + error);
                alert("error");
        }
    });
};

var config={
	type: 'bar',
	data: {
		labels : [],

		datasets : [{
			type: 'line',
			label: '심박수',
			yAxisID: 'sleepline',
			backgroundColor: 'red',
			borderColor: 'rgb(255, 99, 132)',
			
			borderWidth: 1,
			fill: false,
			data: []
		},{
			type: 'line',
			label: '온도',
			yAxisID: 'temline',
			backgroundColor: 'yellow',
			borderColor: 'rgb(245, 209, 91)',
			borderWidth: 1,
			fill: false,
			data:[]
		},{
			type: 'line',
			label: '습도',
			yAxisID: 'humline',
			backgroundColor: 'blue',
			borderColor: 'rgb(91, 181, 245)',
			borderWidth: 1,
			fill : false, 
			data: []
		}]
	},
	options: {
		responsive: false,
		title: {
			display: true,
			text: '수면환경 그래프'
		},
		scales: {
			xAxes: [{
				display: true,
				scaleLabel: { display: true, labelString: '시간' },
			}],
			yAxes: [{
				id: 'sleepline',
				display: true,
				stacked: true,
				scaleLabel: { display: true, labelString: '심박수'},
				ticks : {
					min: 20,
					max: 120, 
					stepSize: 10
				}
			},{
				id: 'temline',
				display: true,
				stacked: true,
				scaleLabel: { display: true, labelString: '온도(C)'},
				position: 'left',
				ticks : {
					min: 0,
					max: 40,
					stepSize: 5
				}
			},{
				id: 'humline',
				display: true,
				stacked: true,
				scaleLabel: { display: true, labelString: '습도'},
				position: 'right',
				ticks : {
					min: 20,
					max: 100,
					stepSize: 5
				}
			}]
		}
	}
};

var ctx = null
var chart = null 
var tick = -1; 

function drawChart() {
	ctx = document.getElementById('canvas').getContext('2d');
	chart = new Chart(ctx, config);
}

function addChartData1(value) {
	tick++;
	var date = new Date();
	chart.data.labels[tick] = date.getHours()+':'+date.getMinutes();
	chart.data.datasets[0].data.push(value);
	chart.update();
}

function addChartData2(value) {
	chart.data.datasets[1].data.push(value);
	chart.update();
}

function addChartData3(value) {
	chart.data.datasets[2].data.push(value);
	chart.update();
}

function hideshow() { // 캔버스 보이기 숨기기 
	if(canvas.style.display == "none") 	canvas.style.display = "block"
	else canvas.style.display = "none"  
}