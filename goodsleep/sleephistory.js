  // API 시작
  function historyStart() {
      var datefrom = document.getElementById('datefrom').value;
      var timefrom = document.getElementById('timefrom').value;
      var dateto = document.getElementById('dateto').value;
      var timeto = document.getElementById('timeto').value;
      var datetime ="/log?from=" +datefrom + "%20" + timefrom +"&to="+ dateto + "%20"+ timeto;
      invokeAPI(datetime);
      emptyTable();
  }
		
  var invokeAPI = function(datetime) {
		
      // 디바이스 조회 URI
      // prod 스테이지 편집기의 맨 위에 있는 "호출 URL/devices"로 대체해야 함
      var API_URI = 'https://6ma0isp3di.execute-api.ap-southeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010' + datetime ; 		        
      $.ajax(API_URI, {
          method: 'GET',
          contentType: "application/json",
           
          success: function (data, status, xhr) {
              document.getElementById("result").innerHTML="조회중";
              var result = JSON.parse(data);
              setDataList(result);  // 성공시, 데이터 출력을 위한 함수 호출
          },
          error: function(xhr,status,e){
            //  document.getElementById("result").innerHTML="Error";
              alert("error");
          }
      });
  };

  var emptyTable = function() {
      $( '#mytable > tbody').empty();
      document.getElementById("result").innerHTML="조회 중입니다.";
  }

  // 데이터 출력을 위한 함수
  function setDataList(json){
      
      for(idx in json.data){
        var tr1 = document.createElement("tr");
        var td1 = document.createElement("td");
        var td2 = document.createElement("td");
        var td3 = document.createElement("td");
        td1.innerText = json.data[idx].timestamp;    
        td2.innerText = json.data[idx].temperature;   
        td3.innerText = json.data[idx].pulse;   
        tr1.appendChild(td1);
        tr1.appendChild(td2);
        tr1.appendChild(td3);
        $('#mytable').append(tr1);
    }

		
      if(data.length>0){
              // 디바이스 목록 조회결과가 있는 경우 데이터가 없습니다 메시지 삭제
          document.getElementById("result").innerHTML="";
      } else if (data.length ==0) {
          document.getElementById("result").innerHTML="No Data";
      }
  }