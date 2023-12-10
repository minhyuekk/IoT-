// API 시작
function Start() {
    var fantime = document.getElementById("userfan").value;
    if(fantime != 0) {
      invokeAPI1();
      setTimeout(function() {
        invokeAPI2();
      }, fantime *60 * 1000);
    }  
}
// API 중지
	
function fanStop() {
      invokeAPI2();
}

var fanON = {
  "tags": [
    {
      "tagName": "fan",
      "tagValue": "ON"
    },
  ]
};

var fanOFF = {
  "tags": [
    {
      "tagName": "fan",
      "tagValue": "OFF"
    },
  ]
};
	
var invokeAPI1 = function() {
    // 디바이스 조회 URI
    // prod 스테이지 편집기의 맨 위에 있는 "호출 URL/devices"로 대체해야 함
    var API_URI = 'https://zywat57ay6.execute-api.ap-northeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010'; 		        
    $.ajax(API_URI, {
        method: 'PUT',
        contentType: "application/json",
        data: JSON.stringify(fanON),
        success: function (data, status, xhr) {
                var result = JSON.parse(data.body);
                console.log("data="+data);
        },
        error: function(xhr,status,e){
                alert("error");
        }
    });
};

var invokeAPI2 = function() {
    // 디바이스 조회 URI
    // prod 스테이지 편집기의 맨 위에 있는 "호출 URL/devices"로 대체해야 함
    var API_URI = 'https://i6pg2z83ce.execute-api.ap-northeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010';             
    $.ajax(API_URI, {
        method: 'PUT',
        contentType: "application/json",
        data: JSON.stringify(fanOFF),
        success: function (data, status, xhr) {
                var result = JSON.parse(data.body);
                console.log("data="+data);
        },
        error: function(xhr,status,e){
                alert("error");
        }
    });
};
