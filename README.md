# SSEngine (Re-implementation)

鏈粨搴撶敤浜庡湪淇濇寔鐩稿悓澶存枃浠舵帴鍙ｇ殑鍓嶆彁涓嬶紝閲嶅啓骞舵浛鎹㈠師鏈?Windows SDK锛坵in32/win64 涓嬬殑棰勭紪璇戝簱锛夈€傜洰鏍囧钩鍙帮細Windows x86/x64锛孡inux x86_64锛宮acOS (x86_64/arm64)銆?
娉ㄦ剰锛歸in32/win64 鐩綍涓殑澶存枃浠朵笌搴撲粛鐒朵繚鐣欙紝鍙綔涓哄鐓ф祴璇曚笌琛屼负鍩哄噯锛屼笉鍐嶇洿鎺ュ弬涓庡悗缁瀯寤恒€?
## 鐩綍缁撴瀯
 - include/        缁熶竴鐨勫叕鍏卞ご鏂囦欢锛堝凡鎵佸钩鍖栧師鏉ョ殑 include/include 缁撴瀯锛?- src/            鍚勬ā鍧楁簮鐮侊紙鍏堝疄鐜?Windows锛屽悗瀹炵幇 Linux锛屾渶鍚?macOS锛?- win32/, win64/  渚涘簲鍟嗕簩杩涘埗涓庡ご鏂囦欢锛堝鐓х敤锛?- CMakeLists.txt  椤跺眰 CMake锛屽伐绋嬩娇鐢?CMake 缁熶竴绠＄悊

## 妯″潡娓呭崟锛堟寜浼樺厛绾э級
1) sdlogger    鏂囦欢/UDP/TCP 鏃ュ織鍣?+ 宸ュ巶瀵煎嚭
2) sdu         甯哥敤宸ュ叿锛堢嚎绋?閿?鏉′欢鍙橀噺/鏃堕棿/鏂囦欢/鍘熷瓙/缃戠粶瀛楄妭搴忕瓑锛?3) sdnet       楂樻€ц兘缃戠粶锛圵indows: IOCP锛汱inux: epoll锛沵acOS: kqueue锛?4) sdpipe      鍩轰簬 sdnet 鐨勬秷鎭?绠￠亾鎶借薄
5) sdconsole   Windows 鎺у埗鍙帮紙鍥哄畾鍖哄煙/婊氬姩鍖?棰滆壊/閿洏鐩戝惉锛?6) sddb        DB 浼氳瘽/鍛戒护/缁撴灉闆嗭紙绗竴闃舵瀹炵幇 Mock/鏈€灏忓彲鐢ㄨ涔夛級
7) sdgate, sdsysteminfo, sddebugviewer  濡備笟鍔￠渶瑕佸啀瀹炵幇

## 寮€鍙戣矾绾垮浘
- 绗?1 闃舵锛圵indows锛?  - 鎼缓宸ョ▼鑴氭墜鏋朵笌瀵圭収娴嬭瘯妗嗘灦
  - 浼樺厛瀹炵幇 sdlogger + sdu锛堟渶灏忓彲鐢ㄥ瓙闆嗭級
  - 瀹炵幇 Windows sdnet锛圛OCP锛夊拰 sdpipe锛岃ˉ鍏呴噾娴嬬敤渚嬶紙涓庝緵搴斿晢搴撳鎷嶏級
  - 瀹炵幇 sdconsole 涓?sddb(Mock) 鐨勬渶灏忚涔夐泦
  - 鍦?Windows CI 涓婃寜妯″潡閫愭瀵规瘮楠岃瘉锛堣涓?閿欒鐮?鍥炶皟椤哄簭/杈圭晫鍦烘櫙锛?- 绗?2 闃舵锛圠inux锛?  - 澶嶇敤澶存枃浠朵笌鎺ュ彛锛岃惤鍦?epoll 鐗堟湰鐨?sdnet 涓?sdpipe锛屽叾浣欐ā鍧楅€傞厤
  - Linux CI 瀵规媿娴嬭瘯
- 绗?3 闃舵锛坢acOS锛?  - 澶嶇敤鍚屼竴濂楀ご鏂囦欢锛岃惤鍦?kqueue 鐗堟湰鐨?sdnet 涓?sdpipe
  - macOS CI 瀵规媿娴嬭瘯

## 瀵规媿涓庢祴璇曠瓥鐣?- 鍚岃繘绋嬪悓鏃跺姞杞解€滀緵搴斿晢搴撯€濆拰鈥滆嚜鐮斿簱鈥濓紝鐢ㄧ浉鍚岀敤渚嬮┍鍔紝姣旇緝锛?  - 杩斿洖鍊间笌閿欒鐮侊紙濡?ESDNetErrCode銆丒SDPipeCode銆丼DDB 閿欒锛?  - 鍥炶皟椤哄簭涓庢椂搴忕獥鍙ｏ紙Run 姝ヨ繘銆佺矘鍖?鍗婂寘銆佸紓甯歌矾寰勶級
  - 鍦板潃/瀛楃涓?闀垮害/缂撳啿鍖虹┖闂寸瓑鍙瀵熺姸鎬?- 涓嶈窇閫氬鎷嶅嵆涓嶆浛鎹€傛€ц兘鍥炲綊鍙﹁璇勪及锛堣繛鎺ユ暟銆佸悶鍚愩€丆PU锛夈€?
## 宸茬煡鍏煎鎬т簨椤?- sdsysteminfo.h/sddebugviewer.h 浣跨敤鐨?`SSSCPVersion` 绫诲瀷鍦ㄤ粨搴撳唴缂哄け瀹氫箟锛屽悗缁皢浠ュ吋瀹瑰ご鎴栧埆鍚嶆柟寮忚ˉ榻愶紙涓嶆敼鍙樺師澶存枃浠跺悕涓庢帴鍙ｏ級銆?- 32/64 浣嶅簱鍛藉悕鍦?win32/win64 涓嶄竴鑷达紙ss*/sd* 鍓嶇紑锛夛紝鏇挎崲鏃堕渶鍒嗗埆鐢熸垚銆?- Windows 鍔ㄦ€佸鍑哄悕灏嗛€氳繃 .def 鎺у埗纭繚 ABI 涓€鑷达紱闈欐€佸簱浣滀负涓棿褰㈡€佸彲鐢ㄤ簬鍗曞厓娴嬭瘯銆?
## 鏋勫缓锛堣剼鎵嬫灦锛?- Windows锛堝悗缁ˉ榻?CI 鑴氭湰涓庣紪璇戣鏄庯級
  ```pwsh
  cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release --target sdlogger
  ```
- 鍏朵粬骞冲彴褰撳墠浠呯敓鎴愬崰浣嶅簱锛屽緟瀹炵幇瀵瑰簲骞冲彴閫昏緫鍚庡啀鍚敤銆?
## 鍙樻洿璁板綍锛堟湰娆★級
- 鎵佸钩 include/include 涓哄崟涓€ include/ 鏍癸紱淇 CMake PUBLIC include 鐩綍
- 瀹炵幇 sdu 瀛愰泦锛歴dnetutils锛堣法骞冲彴锛夈€乻dthread/sdmutex/sdcondition/sdtime/sdfile/sdatomic锛圵indows 浼樺厛锛?- sdlogger锛欳SDLogger 瀹炵幇锛沇indows 涓嬫彁渚涙枃浠?UDP/TCP 鍩虹瀹炵幇
- sdnet锛氭彁渚涢鏋跺伐鍘傦紝鍚庣画鏇挎崲 IOCP 瀹炵幇
- 闆嗘垚 GoogleTest 涓?CTest锛涙柊澧?sdlogger/sdnetutils/sdthread/sdmutex/sdtime/sdfile/sdatomic 鍗曟祴

## 涓嬩竴姝ヨ鍒?1) 鍦?Windows 涓嬪畬鍠?sdlogger 鐨勬枃浠?UDP/TCP 瀹炵幇涓庤涓哄鎷嶆祴璇?2) 瀹炵幇 sdu 鏈€灏忓瓙闆嗭紙绾跨▼/閿?鏃堕棿/鏂囦欢/缃戠粶瀛楄妭搴?鍘熷瓙 绛夛級
3) 璧锋 sdnet(IOCP) + sdpipe 鐨勯鏋朵笌鍏抽敭璺緞娴嬭瘯
4) 鏁村悎 Windows CI锛堟瀯寤恒€佽繍琛屽鎷嶆祴璇曪級

## 文档/Wiki
- 项目 Wiki 入口：wiki/Home.md
- 头文件索引：wiki/Headers.md

