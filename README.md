원본 리포지토리 : https://github.com/MoonSun-v/JesaSang

# 🍚 제사상

- **자체 제작 C++ / DirectX 11 게임 엔진**으로 개발한 3D 게임 프로젝트입니다.
- **프로젝트 기간**: 2026.01.16 ~ 2.13
- **사용 엔진**: 자체 엔진 (C++, DirectX 11)
- **개발 언어**: C++
- **빌드 환경**: Visual Studio   




## 👥 개발팀 구성
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/mob954325">
        <img src="https://avatars.githubusercontent.com/mob954325" width="120px;" alt="mob954325"/>
        <br />
        <sub><b>mob954325</b></sub>
      </a>
      <br />
      엔진 코어<br/>에디터<br/>UI
    </td>
    <td align="center">
      <a href="https://github.com/wooj22">
        <img src="https://avatars.githubusercontent.com/wooj22" width="120px;" alt="wooj22"/>
        <br />
        <sub><b>wooj22</b></sub>
      </a>
      <br />
      렌더러<br/>클라이언트
    </td>
    <td align="center">
      <a href="https://github.com/MoonSun-v">
        <img src="https://avatars.githubusercontent.com/MoonSun-v" width="120px;" alt="MoonSun-v"/>
        <br />
        <sub><b>MoonSun-v</b></sub>
      </a>
      <br />
      물리<br/>애니메이션<br/>클라이언트
    </td>
    <td align="center">
      <a href="https://github.com/Clericcatos">
        <img src="https://avatars.githubusercontent.com/Clericcatos" width="120px;" alt="Clericcatos"/>
        <br />
        <sub><b>Clericcatos</b></sub>
      </a>
      <br />
      사운드<br/>클라이언트
    </td>
  </tr> 
</table>

---

### 개요
해당 프로그램에서는 프로그램 내부에서 Editor모드와 Play모드를 구별합니다.
모든 씬 내부 데이터, 월드 세팅은 json형식으로 저장되고 사용합니다.

Editor모드에서는 Imgui와 rttr를 통해 실시간 조절이 가능합니다.

렌더링은 Deffered Render 방식을 채용합니다.

### 객체 관리
[그림] - slotmap 그림
게임에서 사용하는 모든 객체는 Object 클래스라는 최상위 클래스를 상속받아서 생명관리를 합니다.
ObjectSystem에서 slots 배열에서 관리하고 각 인덱스 별로 generation값을 소유해 포인터를 찾을 때 dangling pointer를 방지합니다.

### 컴포넌트 등록 및 확장성
해당 프로그램에서 모든 컴포넌트는 `Component` 클래스를 상속받은 클래스를 사용합니다.
`Component` 클래스에는 `OnInitialize`, `OnStart`와 같은 가상 이벤트 함수가 있고 이 함수는 System 클래스가 호출합니다.

대표적인 상위 컴포넌트로는 `ScriptComponent`, `RenderComponent`가 있습니다.
위 컴포넌트는 `ScriptStystem`, `RenderSystem`에서 이벤트 호출 순서에 따라 컴포넌트 이벤트가 호출됩니다.

추가로 컴포넌트를 만들려면 `Component`를 상속받은 특정 컴포넌트를 만들고 이를 등록할 시스템 클래스를 추가하여 메인 루프에 호출 순서에 맞게 호출합니다.

### 렌더패스 구조
렌더 패스는 `IRenderPass`를 상속받아서 추가합니다.
각 렌더 패스 클래스는 매개변수로 `ID3D11DeviceContext`, EngineApp에서 정의한 `RenderQueue`, `Camera`를 매개변수로 `Execute()`를 정의합니다.

`RenderComponent`의 `OnRender()`에서 `RenderPass`에 사용할 데이터를 renderQueue의 item으로 추가한 후, 
이 데이터를 가지고 RenderPass에서 설정을 바인딩 후 마지막에 `Draw()`를 호출합니다.


### 엔진구조 
[사진] - 엔진 루프 그림

### 컴포넌트 내부 호출 순서
[사진] - 컴포넌트 이벤트 호출 순서 그림