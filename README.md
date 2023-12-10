# IoT스마트 수면 건강 관리 시스템
IoT 클라우드 플랫폼 기말 과제

<디바이스 개요><br>
IoT스마트 수면 건강 관리 시스템은 사용자의 수면 상태를 기록하고 그 기록을 토대로 수면 환경을 제어해주는 시스템이다.
디바이스에는 온습도센서, 심박도 센서, 비접촉 적외선 온도센서 모듈이 있고, 온습도 센서로 수면 환경의 온도, 심박센서로 사용자의 심박수, 비접촉 온도센서 모듈로 사용자의 체온을 측정한다.
수면 환경을 제어하는 디바이스는 아두이노 쿨링 팬을 사용하였다.
수면 중 온도와 심박수가 일정 수준을 내려가거나 올라가면 자동으로 쿨링팬이 꺼진다.

![1](https://github.com/minhyuekk/IoT-Cloud-Platform/assets/151536962/0be8e6c5-7a11-4b5a-b7fb-09f7cb52f41b)


 - 온도 23도 미만, 심박수 90을 넘어서면 쿨링팬 자동으로 꺼짐
 - 그 외의 상황은 쿨링팬 켜짐(여름이라는 가정하에 제작된 디바이스)

<AWS 백앤드 개요><br>
측정된 값은 AWS의 DynamoDB에 저장되고, 저장된 기록을 앱을 통해 조회할 수 있다.
현재 측정되고 있는 값을 실시간으로 조회할 수 있고, 직접 앱을 통해 쿨링 팬을 켜고 끌 수 있다.

 - DynamoDB: 디바이스에서 측정한 센서 값을 DB에 저장
 - AWS Lambda: 디바이스를 제어하거나 로그를 조회하기 위한 람다함수 생성
 - API Gateway: API를 생성하고 관리

![2](https://github.com/minhyuekk/IoT-Cloud-Platform/assets/151536962/7635515f-4042-428d-8f61-4ec5c9d20039)


<br><br>
<프로젝트 실행>
1. 아두이노 X.509 인증서 발급
2. AWS IoT에 디바이스 등록 및 CSR 등록
3. AWS IoT에 정책 생성 및 인증서에 등록
4. 아두이노 실행 후 arduino_secrets.h에 와이파이 SSID, Password, 엔드포인트, 다운로드 받은 인증서 내용 복붙하고 코드 실행
5. 센서에서 읽힌 값이 시리얼 모니터에 페이로드 형식으로 업로드 되는 것을 확인할 수 있음

<br><br>
<람다함수 설명>
1. ListingDevice : AWS IoT Core에 등록된 디바이스 목록을 조회하기 위한 람다함수
2. GetDevice : 디바이스 현재 상태를 조회하기 위한 람다함수
3. UpdateDevice : 디바이스를 제어하기 위한 람다함수
4. LogDevice : DynamoDB에 저장된 로그를 조회하기 위한 람다함수

<br><br>
<API생성 및 앱 실행>
1. Intellij 툴킷에서 원하는 제어에 맞는 람다함수를 생성하고 AWS API Gateway에서 REST API생성
2. 원하는 데이터를 읽어오기 위한 메소드는 GET을 사용하고,
3. 디바이스를 제어하기 위한 메소드는 PUT을 사용한다.
4. API를 모두 생성 및 배포 후 REST API의 URL을 복사한다.
5. 제어에 따라 리소스 경로를 다르게 하여 링크를 만든다.
6. 안드로이드 스튜디오 앱에서 "MainActivity.java"를 들어간다.
7. 해당 코드의 URL을 사용자에 맞게 수정 후 앱을 실행한다. <br><br>
    private static final String LIST_THINGS_API_URL = "https://zywat57ay6.execute-api.ap-northeast-2.amazonaws.com/prod/devices";<br>
    private static final String THING_SHADOW_API_URL = "https://zywat57ay6.execute-api.ap-northeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010";<br>
    private static final String LIST_LOGS_API_URL = "https://zywat57ay6.execute-api.ap-northeast-2.amazonaws.com/prod/devices/MyMKRWiFi1010/log";<br>
