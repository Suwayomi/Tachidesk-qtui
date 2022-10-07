# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [v2022.6.26](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.6.26) - 2022-10-07

<small>[Compare with v2022.6.25](https://github.com/Suwayomi/Tachidesk-qtui/compare/v2022.6.25...v2022.6.26)</small>

### Fixed
- Fix downward movement in webtoon viewer ([59f9823](https://github.com/Suwayomi/Tachidesk-qtui/commit/59f9823dc84f87f082d07ccadde9a12716ebc5ca) by Norm).
- Fix closing on back in android ([696683c](https://github.com/Suwayomi/Tachidesk-qtui/commit/696683cdfee9202e39e215927a000a978ace0b53) by mgn-norm).


## [v2022.6.25](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.6.25) - 2022-06-25

<small>[Compare with v2022.6.19](https://github.com/Suwayomi/Tachidesk-qtui/compare/v2022.6.19...v2022.6.25)</small>

### Fixed
- Fix android cmake ([b6ae3e3](https://github.com/Suwayomi/Tachidesk-qtui/commit/b6ae3e3aa74ef671f8fb8d3521228938ea516716) by Norm).
- Fix requesting next chapter, use the chapter name for header. some mark read changes ([7243c13](https://github.com/Suwayomi/Tachidesk-qtui/commit/7243c134532d6ffb7d08e378020f4f904629bac5) by Norm).
- Fix page jumping for real this time, fix chapter number sometimes not displaying properly ([db06d5a](https://github.com/Suwayomi/Tachidesk-qtui/commit/db06d5a379d6677723cfb312b4b052d2305036f2) by Norm).

### Removed
- Remove snap mode that was messing with image positioning ([b37385b](https://github.com/Suwayomi/Tachidesk-qtui/commit/b37385bfdb82321b31b2c58e839205739d28d692) by Norm).


## [v2022.6.19](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.6.19) - 2022-06-19

<small>[Compare with v2022.6.18](https://github.com/Suwayomi/Tachidesk-qtui/compare/v2022.6.18...v2022.6.19)</small>

### Added
- Add header bar and chapter number to header bar ([b57a313](https://github.com/Suwayomi/Tachidesk-qtui/commit/b57a313e9e53813364a4d0f65386c2484db0376a) by Norm).

### Fixed
- Fix jump to page triggering more than once ([0f0ae21](https://github.com/Suwayomi/Tachidesk-qtui/commit/0f0ae2182e7f0343b3e6094788a54888f3a26c96) by Norm).


## [v2022.6.18](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.6.18) - 2022-06-18

<small>[Compare with v2022.3.19](https://github.com/Suwayomi/Tachidesk-qtui/compare/v2022.3.19...v2022.6.18)</small>

### Added
- Add cmake install ([75e2f1c](https://github.com/Suwayomi/Tachidesk-qtui/commit/75e2f1c5140b14d3ddc62f1beb3f91c64d068ab4) by Norm).
- Add page index to reader ([609238a](https://github.com/Suwayomi/Tachidesk-qtui/commit/609238a2e6382720affd83af2545e15951e4d6a8) by Norm).
- Add mark previous as read/unread ([3db5b5d](https://github.com/Suwayomi/Tachidesk-qtui/commit/3db5b5dd0558533514fb9c13f313b834156cf4e0) by Norm).


## [v2022.3.19](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.3.19) - 2022-03-19

<small>[Compare with v2022.3.17](https://github.com/Suwayomi/Tachidesk-qtui/compare/v2022.3.17...v2022.3.19)</small>

### Added
- Add search to library ([d1a7d7e](https://github.com/Suwayomi/Tachidesk-qtui/commit/d1a7d7e7f9790cf8de2d377b0c06a8e148bd91fc) by Norm).


## [v2022.3.17](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v2022.3.17) - 2022-03-17

<small>[Compare with v0.0.1-qt6](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.1-qt6...v2022.3.17)</small>

### Added
- Add ability to download chapters from manga details page by clicking icon, and showing the download progress. implement open external browser button ([d3cc4ea](https://github.com/Suwayomi/Tachidesk-qtui/commit/d3cc4ea9d0178ed18c6553a39453d076c6f0eb09) by Norm).


## [v0.0.1-qt6](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.1-qt6) - 2022-03-13

<small>[Compare with v0.0.6](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.6...v0.0.1-qt6)</small>

### Added
- Add ios support in cmake ([21b17ee](https://github.com/Suwayomi/Tachidesk-qtui/commit/21b17eec45135f71ff6b489518145363b24f95f0) by mgn-norm).

### Fixed
- Fix android build ([acadb9b](https://github.com/Suwayomi/Tachidesk-qtui/commit/acadb9b72e5fcb38979bf7e0ae5df1ee38c7f2d5) by Norm).
- Fix websockets not connected to ssl when case doesn't match ([a19af2d](https://github.com/Suwayomi/Tachidesk-qtui/commit/a19af2daaa694b4d98721bed6d31f47ecb5f2422) by Norm).
- Fix genre string, fix crash when updates doesn't recieve a valid reply ([50172ae](https://github.com/Suwayomi/Tachidesk-qtui/commit/50172ae09f787ede96cf371d929b929283501e5d) by Norm).


## [v0.0.6](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.6) - 2021-11-13

<small>[Compare with v0.0.5](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.5...v0.0.6)</small>

### Fixed
- Fix issue with screen not properly going to edge on mobile ([785ce6b](https://github.com/Suwayomi/Tachidesk-qtui/commit/785ce6b7d36b76c580d07e28b8f7f836386ffd7d) by Norm).


## [v0.0.5](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.5) - 2021-11-13

<small>[Compare with v0.0.4](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.4...v0.0.5)</small>

### Added
- Add unread badges ([6d302a4](https://github.com/Suwayomi/Tachidesk-qtui/commit/6d302a4022a46919673110d9e31b9057ea6d6168) by Norm).

### Fixed
- Fix update, add pull down for library update ([7dfb975](https://github.com/Suwayomi/Tachidesk-qtui/commit/7dfb975db571ad83d6cfe1ef9d81212864606807) by Norm).
- Fix using keys to go back ([2f22cba](https://github.com/Suwayomi/Tachidesk-qtui/commit/2f22cbab71a0b05c1b530187466a0a2e6fd5092a) by Norm).


## [v0.0.4](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.4) - 2021-10-17

<small>[Compare with v0.0.3](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.3...v0.0.4)</small>

### Added
- Add date to updates, add clicking manga image takes you to details page ([20cdde6](https://github.com/Suwayomi/Tachidesk-qtui/commit/20cdde6546f1b20b7d4bca715f997e1de6487987) by Norm).
- Add updates to qrc file ([170d117](https://github.com/Suwayomi/Tachidesk-qtui/commit/170d117e2c4e455581632d236e1f01dff2ad866e) by Norm).
- Add qml reload hotkey ([22dc5fd](https://github.com/Suwayomi/Tachidesk-qtui/commit/22dc5fdf9dbd5e5c7e9fe534f11edd354870c825) by Norm).

### Fixed
- Fix interface models warnings ([7fe1e74](https://github.com/Suwayomi/Tachidesk-qtui/commit/7fe1e74cbda748058b932f0ed9cf74d49bfab34c) by Norm).
- Fix source image locations and add to resource file ([cab519c](https://github.com/Suwayomi/Tachidesk-qtui/commit/cab519cfd842d5b60e1ef0483a78ec73afe3ac6b) by Norm).
- Fix search ([45d231a](https://github.com/Suwayomi/Tachidesk-qtui/commit/45d231aaa5554a39865ea9065add53a6cd65787a) by Norm).


## [v0.0.3](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.3) - 2021-09-28

<small>[Compare with v0.0.2](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.2...v0.0.3)</small>

### Fixed
- Fix up updates page, read chapter, and download ([80bfa41](https://github.com/Suwayomi/Tachidesk-qtui/commit/80bfa41a715f1b41a1e83f37cf87034fe2921f64) by Norm).


## [v0.0.2](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.2) - 2021-09-22

<small>[Compare with v0.0.1](https://github.com/Suwayomi/Tachidesk-qtui/compare/v0.0.1...v0.0.2)</small>

### Added
- Add gcc to dependency list ([aa2aa77](https://github.com/Suwayomi/Tachidesk-qtui/commit/aa2aa779975791bbab2d57efff59ce13b09736b7) by Aria Moradi).
- Add loading indicator for loading new chapters ([5843bea](https://github.com/Suwayomi/Tachidesk-qtui/commit/5843bea6ce522d32fb17a8b3e4132a4b815fc267) by Norm).
- Add busy indicator for images that are loading ([6dbed34](https://github.com/Suwayomi/Tachidesk-qtui/commit/6dbed342313f2e784b717ab93a1156abfae846a1) by Norm).
- Add ssl protocol support for websocket ([d24c301](https://github.com/Suwayomi/Tachidesk-qtui/commit/d24c301563662e3a84f02c95992bb4d0f14db90b) by Norm).

### Fixed
- Fix chapters model update, add some padding to the manga details icon ([2c46ccd](https://github.com/Suwayomi/Tachidesk-qtui/commit/2c46ccdfb84af46bbd48530678fae54b5ea08963) by Norm).
- Fix weirdly resizing image on the details page ([88c656a](https://github.com/Suwayomi/Tachidesk-qtui/commit/88c656a80f2e9805b9c79c55cd3774a6a6806dde) by Norm).

### Removed
- Remove unused code ([0afcf49](https://github.com/Suwayomi/Tachidesk-qtui/commit/0afcf49c34486210361b1f309982173f45c5eb77) by Norm).


## [v0.0.1](https://github.com/Suwayomi/Tachidesk-qtui/releases/tag/v0.0.1) - 2021-09-17

<small>[Compare with first commit](https://github.com/Suwayomi/Tachidesk-qtui/compare/bbe758431d420df1936c705e6b5452c5c793311f...v0.0.1)</small>

### Added
- Add full screen to reader, add openssl for android ([599287b](https://github.com/Suwayomi/Tachidesk-qtui/commit/599287b4b1cd943ca600012871464b5a7e7970d3) by Norm).
- Add android icon/manifest file ([50d4725](https://github.com/Suwayomi/Tachidesk-qtui/commit/50d47258d58c88b9dab6b7860f23ab9b6e41b317) by Norm).
- Add a button for resuming from last unread chapter, add long press for download and marking as read or unread ([03aa4ea](https://github.com/Suwayomi/Tachidesk-qtui/commit/03aa4eaddacde472821732d697067d8da481744a) by Norm).
- Add terrible basic download page, touch up details page readd accidently removed search (broken?), remove unused code ([e74fb5f](https://github.com/Suwayomi/Tachidesk-qtui/commit/e74fb5f7b437014ab1e0d2e78814d3de39e29d05) by Norm).
- Add option to download all or unread chapters ([52ea3f9](https://github.com/Suwayomi/Tachidesk-qtui/commit/52ea3f97b0e369ca264596226a562a8eaf52eaa9) by Norm).
- Add nsfw filter for extensions ([b98cf19](https://github.com/Suwayomi/Tachidesk-qtui/commit/b98cf19a1e84c696ec5ab7b337fe83fef5684bf5) by Norm).
- Add basic search to sources, no multiple pages ([489f607](https://github.com/Suwayomi/Tachidesk-qtui/commit/489f607f37f5e6f69f7e6eb0f5e8417d43f00cb3) by Norm).
- Add transitions, bit more color to other pages ([0c24307](https://github.com/Suwayomi/Tachidesk-qtui/commit/0c243077d2123238f8f1c191f07ea60798c951bd) by Norm).
- Add option for base url ([b873096](https://github.com/Suwayomi/Tachidesk-qtui/commit/b8730964056009a5d812e82c1dadfe2688c0e6fe) by Norm).
- Add continuous loading for chapters (breaks marking chapters read at end) ([256b3d8](https://github.com/Suwayomi/Tachidesk-qtui/commit/256b3d8aec5a2a8db6c71f2d4e10e7ff736c291d) by Norm).
- Add language setting for sources, misc fixes ([545e372](https://github.com/Suwayomi/Tachidesk-qtui/commit/545e37293b3a62c53c649256dc7dbafc465d6da4) by Norm).
- Add chapter model to properly load chapters, add missing qrc line ([bf9e25a](https://github.com/Suwayomi/Tachidesk-qtui/commit/bf9e25ab53be0b94901da1535e4fb8123801fbbc) by Norm).
- Add ability to add/remove manga from library ([3ea0bf4](https://github.com/Suwayomi/Tachidesk-qtui/commit/3ea0bf4402f8a2cf3bf57fb2c8dd0c9e18e94877) by Norm).
- Add details to library/source library ([5517d7c](https://github.com/Suwayomi/Tachidesk-qtui/commit/5517d7c4234ec4c8a2f21f45175c06aca1a18490) by Norm).
- Add missing resource to qrc file ([bcd1f19](https://github.com/Suwayomi/Tachidesk-qtui/commit/bcd1f19cddb79ab41eef860f4c9b75b0b0608c34) by Norm).
- Add search for extensions, start work on sources (wip) ([f50a11a](https://github.com/Suwayomi/Tachidesk-qtui/commit/f50a11a311e23d2d72d5839e8660067944ca1ba8) by Norm).

### Fixed
- Fix marking chapter as read, bit of work on library styling, update readme ([fa06919](https://github.com/Suwayomi/Tachidesk-qtui/commit/fa069195532f5b60ae12afae007e3e0494771859) by Norm).
- Fix chapter download ([ed9784a](https://github.com/Suwayomi/Tachidesk-qtui/commit/ed9784a9c6ca0d4b7635a05ac6b9829693a8a0a8) by Norm).
- Fix hardcoded ws url, stylize downloads a tiny bit ([855b0cd](https://github.com/Suwayomi/Tachidesk-qtui/commit/855b0cd49cd07475d229eb094cb9257c05c43dcd) by Norm).
- Fix crash when requesting chapters, properly mark last chapter as read ([83503be](https://github.com/Suwayomi/Tachidesk-qtui/commit/83503bea68fa78f622cdccde3ac8a10601789c37) by Norm).
- Fix read/unread ([0f51085](https://github.com/Suwayomi/Tachidesk-qtui/commit/0f510858d1d8ba53cfbb14fd1d37962e445067a4) by Norm).
- Fix patch, change chapter selection styling ([0bdf26f](https://github.com/Suwayomi/Tachidesk-qtui/commit/0bdf26f922ae7bc83068c99dce915ecaa6ef862b) by Norm).
- Fix seg fault when sending patch ([b0882c9](https://github.com/Suwayomi/Tachidesk-qtui/commit/b0882c9a42b1d733ff04ba547f8a83d5f4cadaf9) by Norm).
- Fix default hostname ([fe099b4](https://github.com/Suwayomi/Tachidesk-qtui/commit/fe099b4851aa03fb0415d66881c9a2683e41e742) by Norm).
- Fix gcc build, add contribution file ([a173be3](https://github.com/Suwayomi/Tachidesk-qtui/commit/a173be3cc772ac7a0d7c7977681c9ef5c1ada806) by Norm).
- Fix bug where chapter list would override library ([97c0d1d](https://github.com/Suwayomi/Tachidesk-qtui/commit/97c0d1d15dbee9b4d8c45053e7eccb384de00cc1) by Norm).

### Removed
- Remove port and base url, can be entered as just a url ([7468d19](https://github.com/Suwayomi/Tachidesk-qtui/commit/7468d195fc9fde43c0d0d0b22c0ded9ba0c4b387) by Norm).
- Remove debugging messages ([a117b46](https://github.com/Suwayomi/Tachidesk-qtui/commit/a117b466c209ece5676c4874647b888dd873dd9f) by Norm).


