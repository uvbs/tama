
PGMAN SDK library


pgmanSDK_redistribution 	: pgmanSDK library

pgmanSDK_sample				: pgmanSDK sample source

pgmanSDK_sample.apk			: pgmanSDK samnple running file

PGmanSDK인증 API 가이드 	: pgmanSDK setup guide


pgmanSDK_redistribution 폴더와 pgmanSDK_sample 폴더를 import

=================================================================================

ver6 변경사항

-	테스터 계정 : 결제 테스트 메소드를 사용하지 않아도 결재 테스트를 진행할수 있습니다 
	(청구되지 않습니다 )

ver5 변경사항

-	테스터 계정 : 개발자 페이지에 앱이 등록대기되어 있을경우(승인나지 않은 상태) 결제 테스트 메소드를 사용하여 결재 테스트를 진행할수 있습니다 
	(청구되지 않습니다 )

ver4 변경사항

-	테스터 계정 : 개발자 페이지에 테스터 등록이 되있을경우 인앱 결재시 0 이라도 결재가 진행 됩니다
	( 청구되지 않습니다 )

	
ver3 변경사항

-	인앱 결제시 com.pgman.inappbilling.pgmanstore_IAB_Helper를 사용하도록 변경되었습니다

-	com.pgman.inappbilling.pgmanstore_IAB_Helper.SetupHelper 한번 초기화 이후 
	com.pgman.inappbilling.pgmanstore_IAB_Helper.launchPurchaseFlow 만 사용하여 진행 합니다


ver2 변경사항

-	인앱결재시 반드시 초기화를 거치도록 수정 ( 에러코드 -94 )

-	인앱결재 인증시 유저 정보를 가져오는 동안 디스플레이 표시
