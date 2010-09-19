
/*

//
//  Language IDs.
//
//  The following two combinations of primary language ID and
//  sublanguage ID have special semantics:
//
//    Primary Language ID   Sublanguage ID      Result
//    -------------------   ---------------     ------------------------
//    LANG_NEUTRAL          SUBLANG_NEUTRAL     Language neutral
//    LANG_NEUTRAL          SUBLANG_DEFAULT     User default language
//    LANG_NEUTRAL          SUBLANG_SYS_DEFAULT System default language
//    LANG_INVARIANT        SUBLANG_NEUTRAL     Invariant locale
//

//
//  Primary language IDs.
//

#define LANG_NEUTRAL                     0x00
#define LANG_INVARIANT                   0x7f

#define LANG_AFRIKAANS                   0x36
#define LANG_ALBANIAN                    0x1c
#define LANG_ALSATIAN                    0x84
#define LANG_AMHARIC                     0x5e
#define LANG_ARABIC                      0x01
#define LANG_ARMENIAN                    0x2b
#define LANG_ASSAMESE                    0x4d
#define LANG_AZERI                       0x2c
#define LANG_BASHKIR                     0x6d
#define LANG_BASQUE                      0x2d
#define LANG_BELARUSIAN                  0x23
#define LANG_BENGALI                     0x45
#define LANG_BRETON                      0x7e
#define LANG_BOSNIAN                     0x1a   // Use with SUBLANG_BOSNIAN_* Sublanguage IDs
#define LANG_BOSNIAN_NEUTRAL           0x781a   // Use with the ConvertDefaultLocale function
#define LANG_BULGARIAN                   0x02
#define LANG_CATALAN                     0x03
#define LANG_CHINESE                     0x04   // Use with SUBLANG_CHINESE_* Sublanguage IDs
#define LANG_CHINESE_SIMPLIFIED          0x04   // Use with the ConvertDefaultLocale function
#define LANG_CHINESE_TRADITIONAL       0x7c04   // Use with the ConvertDefaultLocale function
#define LANG_CORSICAN                    0x83
#define LANG_CROATIAN                    0x1a
#define LANG_CZECH                       0x05
#define LANG_DANISH                      0x06
#define LANG_DARI                        0x8c
#define LANG_DIVEHI                      0x65
#define LANG_DUTCH                       0x13
#define LANG_ENGLISH                     0x09
#define LANG_ESTONIAN                    0x25
#define LANG_FAEROESE                    0x38
#define LANG_SI                       0x29   // Deprecated: use LANG_PERSIAN instead
#define LANG_FILIPINO                    0x64
#define LANG_FINNISH                     0x0b
#define LANG_FRENCH                      0x0c
#define LANG_FRISIAN                     0x62
#define LANG_GALICIAN                    0x56
#define LANG_GEORGIAN                    0x37
#define LANG_GERMAN                      0x07
#define LANG_GREEK                       0x08
#define LANG_GREENLANDIC                 0x6f
#define LANG_GUJARATI                    0x47
#define LANG_HAUSA                       0x68
#define LANG_HEBREW                      0x0d
#define LANG_HINDI                       0x39
#define LANG_HUNGARIAN                   0x0e
#define LANG_ICELANDIC                   0x0f
#define LANG_IGBO                        0x70
#define LANG_INDONESIAN                  0x21
#define LANG_INUKTITUT                   0x5d
#define LANG_IRISH                       0x3c   // Use with the SUBLANG_IRISH_IRELAND Sublanguage ID
#define LANG_ITALIAN                     0x10
#define LANG_JAPANESE                    0x11
#define LANG_KANNADA                     0x4b
#define LANG_KASHMIRI                    0x60
#define LANG_KAZAK                       0x3f
#define LANG_KHMER                       0x53
#define LANG_KICHE                       0x86
#define LANG_KINYARWANDA                 0x87
#define LANG_KONKANI                     0x57
#define LANG_KOREAN                      0x12
#define LANG_KYRGYZ                      0x40
#define LANG_LAO                         0x54
#define LANG_LATVIAN                     0x26
#define LANG_LITHUANIAN                  0x27
#define LANG_LOWER_SORBIAN               0x2e
#define LANG_LUXEMBOURGISH               0x6e
#define LANG_MACEDONIAN                  0x2f   // the Former Yugoslav Republic of Macedonia
#define LANG_MALAY                       0x3e
#define LANG_MALAYALAM                   0x4c
#define LANG_MALTESE                     0x3a
#define LANG_MANIPURI                    0x58
#define LANG_MAORI                       0x81
#define LANG_MAPUDUNGUN                  0x7a
#define LANG_MARATHI                     0x4e
#define LANG_MOHAWK                      0x7c
#define LANG_MONGOLIAN                   0x50
#define LANG_NEPALI                      0x61
#define LANG_NORWEGIAN                   0x14
#define LANG_OCCITAN                     0x82
#define LANG_ORIYA                       0x48
#define LANG_PASHTO                      0x63
#define LANG_PERSIAN                     0x29
#define LANG_POLISH                      0x15
#define LANG_PORTUGUESE                  0x16
#define LANG_PUNJABI                     0x46
#define LANG_QUECHUA                     0x6b
#define LANG_ROMANIAN                    0x18
#define LANG_ROMANSH                     0x17
#define LANG_RUSSIAN                     0x19
#define LANG_SAMI                        0x3b
#define LANG_SANSKRIT                    0x4f
#define LANG_SERBIAN                     0x1a   // Use with the SUBLANG_SERBIAN_* Sublanguage IDs
#define LANG_SERBIAN_NEUTRAL           0x7c1a   // Use with the ConvertDefaultLocale function
#define LANG_SINDHI                      0x59
#define LANG_SINHALESE                   0x5b
#define LANG_SLOVAK                      0x1b
#define LANG_SLOVENIAN                   0x24
#define LANG_SOTHO                       0x6c
#define LANG_SPANISH                     0x0a
#define LANG_SWAHILI                     0x41
#define LANG_SWEDISH                     0x1d
#define LANG_SYRIAC                      0x5a
#define LANG_TAJIK                       0x28
#define LANG_TAMAZIGHT                   0x5f
#define LANG_TAMIL                       0x49
#define LANG_TATAR                       0x44
#define LANG_TELUGU                      0x4a
#define LANG_THAI                        0x1e
#define LANG_TIBETAN                     0x51
#define LANG_TIGRIGNA                    0x73
#define LANG_TSWANA                      0x32
#define LANG_TURKISH                     0x1f
#define LANG_TURKMEN                     0x42
#define LANG_UIGHUR                      0x80
#define LANG_UKRAINIAN                   0x22
#define LANG_UPPER_SORBIAN               0x2e
#define LANG_URDU                        0x20
#define LANG_UZBEK                       0x43
#define LANG_VIETNAMESE                  0x2a
#define LANG_WELSH                       0x52
#define LANG_WOLOF                       0x88
#define LANG_XHOSA                       0x34
#define LANG_YAKUT                       0x85
#define LANG_YI                          0x78
#define LANG_YORUBA                      0x6a
#define LANG_ZULU                        0x35

//
//  Sublanguage IDs.
//
//  The name immediately following SUBLANG_ dictates which primary
//  language ID that sublanguage ID can be combined with to form a
//  valid language ID.
//

#define SUBLANG_NEUTRAL                             0x00    // language neutral
#define SUBLANG_DEFAULT                             0x01    // user default
#define SUBLANG_SYS_DEFAULT                         0x02    // system default
#define SUBLANG_CUSTOM_DEFAULT                      0x03    // default custom language/locale
#define SUBLANG_CUSTOM_UNSPECIFIED                  0x04    // custom language/locale
#define SUBLANG_UI_CUSTOM_DEFAULT                   0x05    // Default custom MUI language/locale


#define SUBLANG_AFRIKAANS_SOUTH_AFRICA              0x01    // Afrikaans (South Africa) 0x0436 af-ZA
#define SUBLANG_ALBANIAN_ALBANIA                    0x01    // Albanian (Albania) 0x041c sq-AL
#define SUBLANG_ALSATIAN_FRANCE                     0x01    // Alsatian (France) 0x0484
#define SUBLANG_AMHARIC_ETHIOPIA                    0x01    // Amharic (Ethiopia) 0x045e
#define SUBLANG_ARABIC_SAUDI_ARABIA                 0x01    // Arabic (Saudi Arabia)
#define SUBLANG_ARABIC_IRAQ                         0x02    // Arabic (Iraq)
#define SUBLANG_ARABIC_EGYPT                        0x03    // Arabic (Egypt)
#define SUBLANG_ARABIC_LIBYA                        0x04    // Arabic (Libya)
#define SUBLANG_ARABIC_ALGERIA                      0x05    // Arabic (Algeria)
#define SUBLANG_ARABIC_MOROCCO                      0x06    // Arabic (Morocco)
#define SUBLANG_ARABIC_TUNISIA                      0x07    // Arabic (Tunisia)
#define SUBLANG_ARABIC_OMAN                         0x08    // Arabic (Oman)
#define SUBLANG_ARABIC_YEMEN                        0x09    // Arabic (Yemen)
#define SUBLANG_ARABIC_SYRIA                        0x0a    // Arabic (Syria)
#define SUBLANG_ARABIC_JORDAN                       0x0b    // Arabic (Jordan)
#define SUBLANG_ARABIC_LEBANON                      0x0c    // Arabic (Lebanon)
#define SUBLANG_ARABIC_KUWAIT                       0x0d    // Arabic (Kuwait)
#define SUBLANG_ARABIC_UAE                          0x0e    // Arabic (U.A.E)
#define SUBLANG_ARABIC_BAHRAIN                      0x0f    // Arabic (Bahrain)
#define SUBLANG_ARABIC_QATAR                        0x10    // Arabic (Qatar)
#define SUBLANG_ARMENIAN_ARMENIA                    0x01    // Armenian (Armenia) 0x042b hy-AM
#define SUBLANG_ASSAMESE_INDIA                      0x01    // Assamese (India) 0x044d
#define SUBLANG_AZERI_LATIN                         0x01    // Azeri (Latin)
#define SUBLANG_AZERI_CYRILLIC                      0x02    // Azeri (Cyrillic)
#define SUBLANG_BASHKIR_RUSSIA                      0x01    // Bashkir (Russia) 0x046d ba-RU
#define SUBLANG_BASQUE_BASQUE                       0x01    // Basque (Basque) 0x042d eu-ES
#define SUBLANG_BELARUSIAN_BELARUS                  0x01    // Belarusian (Belarus) 0x0423 be-BY
#define SUBLANG_BENGALI_INDIA                       0x01    // Bengali (India)
#define SUBLANG_BENGALI_BANGLADESH                  0x02    // Bengali (Bangladesh)
#define SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN    0x05    // Bosnian (Bosnia and Herzegovina - Latin) 0x141a bs-BA-Latn
#define SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC 0x08    // Bosnian (Bosnia and Herzegovina - Cyrillic) 0x201a bs-BA-Cyrl
#define SUBLANG_BRETON_FRANCE                       0x01    // Breton (France) 0x047e
#define SUBLANG_BULGARIAN_BULGARIA                  0x01    // Bulgarian (Bulgaria) 0x0402
#define SUBLANG_CATALAN_CATALAN                     0x01    // Catalan (Catalan) 0x0403
#define SUBLANG_CHINESE_TRADITIONAL                 0x01    // Chinese (Taiwan) 0x0404 zh-TW
#define SUBLANG_CHINESE_SIMPLIFIED                  0x02    // Chinese (PR China) 0x0804 zh-CN
#define SUBLANG_CHINESE_HONGKONG                    0x03    // Chinese (Hong Kong S.A.R., P.R.C.) 0x0c04 zh-HK
#define SUBLANG_CHINESE_SINGAPORE                   0x04    // Chinese (Singapore) 0x1004 zh-SG
#define SUBLANG_CHINESE_MACAU                       0x05    // Chinese (Macau S.A.R.) 0x1404 zh-MO
#define SUBLANG_CORSICAN_FRANCE                     0x01    // Corsican (France) 0x0483
#define SUBLANG_CZECH_CZECH_REPUBLIC                0x01    // Czech (Czech Republic) 0x0405
#define SUBLANG_CROATIAN_CROATIA                    0x01    // Croatian (Croatia)
#define SUBLANG_CROATIAN_BOSNIA_HERZEGOVINA_LATIN   0x04    // Croatian (Bosnia and Herzegovina - Latin) 0x101a hr-BA
#define SUBLANG_DANISH_DENMARK                      0x01    // Danish (Denmark) 0x0406
#define SUBLANG_DARI_AFGHANISTAN                    0x01    // Dari (Afghanistan)
#define SUBLANG_DIVEHI_MALDIVES                     0x01    // Divehi (Maldives) 0x0465 div-MV
#define SUBLANG_DUTCH                               0x01    // Dutch
#define SUBLANG_DUTCH_BELGIAN                       0x02    // Dutch (Belgian)
#define SUBLANG_ENGLISH_US                          0x01    // English (USA)
#define SUBLANG_ENGLISH_UK                          0x02    // English (UK)
#define SUBLANG_ENGLISH_AUS                         0x03    // English (Australian)
#define SUBLANG_ENGLISH_CAN                         0x04    // English (Canadian)
#define SUBLANG_ENGLISH_NZ                          0x05    // English (New Zealand)
#define SUBLANG_ENGLISH_EIRE                        0x06    // English (Irish)
#define SUBLANG_ENGLISH_SOUTH_AFRICA                0x07    // English (South Africa)
#define SUBLANG_ENGLISH_JAMAICA                     0x08    // English (Jamaica)
#define SUBLANG_ENGLISH_CARIBBEAN                   0x09    // English (Caribbean)
#define SUBLANG_ENGLISH_BELIZE                      0x0a    // English (Belize)
#define SUBLANG_ENGLISH_TRINIDAD                    0x0b    // English (Trinidad)
#define SUBLANG_ENGLISH_ZIMBABWE                    0x0c    // English (Zimbabwe)
#define SUBLANG_ENGLISH_PHILIPPINES                 0x0d    // English (Philippines)
#define SUBLANG_ENGLISH_INDIA                       0x10    // English (India)
#define SUBLANG_ENGLISH_MALAYSIA                    0x11    // English (Malaysia)
#define SUBLANG_ENGLISH_SINGAPORE                   0x12    // English (Singapore)
#define SUBLANG_ESTONIAN_ESTONIA                    0x01    // Estonian (Estonia) 0x0425 et-EE
#define SUBLANG_FAEROESE_OE_ISLANDS              0x01    // oese (oe Islands) 0x0438 fo-FO
#define SUBLANG_FILIPINO_PHILIPPINES                0x01    // Filipino (Philippines) 0x0464 fil-PH
#define SUBLANG_FINNISH_FINLAND                     0x01    // Finnish (Finland) 0x040b
#define SUBLANG_FRENCH                              0x01    // French
#define SUBLANG_FRENCH_BELGIAN                      0x02    // French (Belgian)
#define SUBLANG_FRENCH_CANADIAN                     0x03    // French (Canadian)
#define SUBLANG_FRENCH_SWISS                        0x04    // French (Swiss)
#define SUBLANG_FRENCH_LUXEMBOURG                   0x05    // French (Luxembourg)
#define SUBLANG_FRENCH_MONACO                       0x06    // French (Monaco)
#define SUBLANG_FRISIAN_NETHERLANDS                 0x01    // Frisian (Netherlands) 0x0462 fy-NL
#define SUBLANG_GALICIAN_GALICIAN                   0x01    // Galician (Galician) 0x0456 gl-ES
#define SUBLANG_GEORGIAN_GEORGIA                    0x01    // Georgian (Georgia) 0x0437 ka-GE
#define SUBLANG_GERMAN                              0x01    // German
#define SUBLANG_GERMAN_SWISS                        0x02    // German (Swiss)
#define SUBLANG_GERMAN_AUSTRIAN                     0x03    // German (Austrian)
#define SUBLANG_GERMAN_LUXEMBOURG                   0x04    // German (Luxembourg)
#define SUBLANG_GERMAN_LIECHTENSTEIN                0x05    // German (Liechtenstein)
#define SUBLANG_GREEK_GREECE                        0x01    // Greek (Greece)
#define SUBLANG_GREENLANDIC_GREENLAND               0x01    // Greenlandic (Greenland) 0x046f kl-GL
#define SUBLANG_GUJARATI_INDIA                      0x01    // Gujarati (India (Gujarati Script)) 0x0447 gu-IN
#define SUBLANG_HAUSA_NIGERIA_LATIN                 0x01    // Hausa (Latin, Nigeria) 0x0468 ha-NG-Latn
#define SUBLANG_HEBREW_ISRAEL                       0x01    // Hebrew (Israel) 0x040d
#define SUBLANG_HINDI_INDIA                         0x01    // Hindi (India) 0x0439 hi-IN
#define SUBLANG_HUNGARIAN_HUNGARY                   0x01    // Hungarian (Hungary) 0x040e
#define SUBLANG_ICELANDIC_ICELAND                   0x01    // Icelandic (Iceland) 0x040f
#define SUBLANG_IGBO_NIGERIA                        0x01    // Igbo (Nigeria) 0x0470 ig-NG
#define SUBLANG_INDONESIAN_INDONESIA                0x01    // Indonesian (Indonesia) 0x0421 id-ID
#define SUBLANG_INUKTITUT_CANADA                    0x01    // Inuktitut (Syllabics) (Canada) 0x045d iu-CA-Cans
#define SUBLANG_INUKTITUT_CANADA_LATIN              0x02    // Inuktitut (Canada - Latin)
#define SUBLANG_IRISH_IRELAND                       0x02    // Irish (Ireland)
#define SUBLANG_ITALIAN                             0x01    // Italian
#define SUBLANG_ITALIAN_SWISS                       0x02    // Italian (Swiss)
#define SUBLANG_JAPANESE_JAPAN                      0x01    // Japanese (Japan) 0x0411
#define SUBLANG_KANNADA_INDIA                       0x01    // Kannada (India (Kannada Script)) 0x044b kn-IN
#define SUBLANG_KASHMIRI_SASIA                      0x02    // Kashmiri (South Asia)
#define SUBLANG_KASHMIRI_INDIA                      0x02    // For app compatibility only
#define SUBLANG_KAZAK_KAZAKHSTAN                    0x01    // Kazakh (Kazakhstan) 0x043f kk-KZ
#define SUBLANG_KHMER_CAMBODIA                      0x01    // Khmer (Cambodia) 0x0453 kh-KH
#define SUBLANG_KICHE_GUATEMALA                     0x01    // K'iche (Guatemala)
#define SUBLANG_KINYARWANDA_RWANDA                  0x01    // Kinyarwanda (Rwanda) 0x0487 rw-RW
#define SUBLANG_KONKANI_INDIA                       0x01    // Konkani (India) 0x0457 kok-IN
#define SUBLANG_KOREAN                              0x01    // Korean (Extended Wansung)
#define SUBLANG_KYRGYZ_KYRGYZSTAN                   0x01    // Kyrgyz (Kyrgyzstan) 0x0440 ky-KG
#define SUBLANG_LAO_LAO                             0x01    // Lao (Lao PDR) 0x0454 lo-LA
#define SUBLANG_LATVIAN_LATVIA                      0x01    // Latvian (Latvia) 0x0426 lv-LV
#define SUBLANG_LITHUANIAN                          0x01    // Lithuanian
#define SUBLANG_LOWER_SORBIAN_GERMANY               0x02    // Lower Sorbian (Germany) 0x082e wee-DE
#define SUBLANG_LUXEMBOURGISH_LUXEMBOURG            0x01    // Luxembourgish (Luxembourg) 0x046e lb-LU
#define SUBLANG_MACEDONIAN_MACEDONIA                0x01    // Macedonian (Macedonia (FYROM)) 0x042f mk-MK
#define SUBLANG_MALAY_MALAYSIA                      0x01    // Malay (Malaysia)
#define SUBLANG_MALAY_BRUNEI_DARUSSALAM             0x02    // Malay (Brunei Darussalam)
#define SUBLANG_MALAYALAM_INDIA                     0x01    // Malayalam (India (Malayalam Script) ) 0x044c ml-IN
#define SUBLANG_MALTESE_MALTA                       0x01    // Maltese (Malta) 0x043a mt-MT
#define SUBLANG_MAORI_NEW_ZEALAND                   0x01    // Maori (New Zealand) 0x0481 mi-NZ
#define SUBLANG_MAPUDUNGUN_CHILE                    0x01    // Mapudungun (Chile) 0x047a arn-CL
#define SUBLANG_MARATHI_INDIA                       0x01    // Marathi (India) 0x044e mr-IN
#define SUBLANG_MOHAWK_MOHAWK                       0x01    // Mohawk (Mohawk) 0x047c moh-CA
#define SUBLANG_MONGOLIAN_CYRILLIC_MONGOLIA         0x01    // Mongolian (Cyrillic, Mongolia)
#define SUBLANG_MONGOLIAN_PRC                       0x02    // Mongolian (PRC)
#define SUBLANG_NEPALI_INDIA                        0x02    // Nepali (India)
#define SUBLANG_NEPALI_NEPAL                        0x01    // Nepali (Nepal) 0x0461 ne-NP
#define SUBLANG_NORWEGIAN_BOKMAL                    0x01    // Norwegian (Bokmal)
#define SUBLANG_NORWEGIAN_NYNORSK                   0x02    // Norwegian (Nynorsk)
#define SUBLANG_OCCITAN_FRANCE                      0x01    // Occitan (France) 0x0482 oc-FR
#define SUBLANG_ORIYA_INDIA                         0x01    // Oriya (India (Oriya Script)) 0x0448 or-IN
#define SUBLANG_PASHTO_AFGHANISTAN                  0x01    // Pashto (Afghanistan)
#define SUBLANG_PERSIAN_IRAN                        0x01    // Persian (Iran) 0x0429 fa-IR
#define SUBLANG_POLISH_POLAND                       0x01    // Polish (Poland) 0x0415
#define SUBLANG_PORTUGUESE                          0x02    // Portuguese
#define SUBLANG_PORTUGUESE_BRAZILIAN                0x01    // Portuguese (Brazilian)
#define SUBLANG_PUNJABI_INDIA                       0x01    // Punjabi (India (Gurmukhi Script)) 0x0446 pa-IN
#define SUBLANG_QUECHUA_BOLIVIA                     0x01    // Quechua (Bolivia)
#define SUBLANG_QUECHUA_ECUADOR                     0x02    // Quechua (Ecuador)
#define SUBLANG_QUECHUA_PERU                        0x03    // Quechua (Peru)
#define SUBLANG_ROMANIAN_ROMANIA                    0x01    // Romanian (Romania) 0x0418
#define SUBLANG_ROMANSH_SWITZERLAND                 0x01    // Romansh (Switzerland) 0x0417 rm-CH
#define SUBLANG_RUSSIAN_RUSSIA                      0x01    // Russian (Russia) 0x0419
#define SUBLANG_SAMI_NORTHERN_NORWAY                0x01    // Northern Sami (Norway)
#define SUBLANG_SAMI_NORTHERN_SWEDEN                0x02    // Northern Sami (Sweden)
#define SUBLANG_SAMI_NORTHERN_FINLAND               0x03    // Northern Sami (Finland)
#define SUBLANG_SAMI_LULE_NORWAY                    0x04    // Lule Sami (Norway)
#define SUBLANG_SAMI_LULE_SWEDEN                    0x05    // Lule Sami (Sweden)
#define SUBLANG_SAMI_SOUTHERN_NORWAY                0x06    // Southern Sami (Norway)
#define SUBLANG_SAMI_SOUTHERN_SWEDEN                0x07    // Southern Sami (Sweden)
#define SUBLANG_SAMI_SKOLT_FINLAND                  0x08    // Skolt Sami (Finland)
#define SUBLANG_SAMI_INARI_FINLAND                  0x09    // Inari Sami (Finland)
#define SUBLANG_SANSKRIT_INDIA                      0x01    // Sanskrit (India) 0x044f sa-IN
#define SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_LATIN    0x06    // Serbian (Bosnia and Herzegovina - Latin)
#define SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_CYRILLIC 0x07    // Serbian (Bosnia and Herzegovina - Cyrillic)
#define SUBLANG_SERBIAN_CROATIA                     0x01    // Croatian (Croatia) 0x041a hr-HR
#define SUBLANG_SERBIAN_LATIN                       0x02    // Serbian (Latin)
#define SUBLANG_SERBIAN_CYRILLIC                    0x03    // Serbian (Cyrillic)
#define SUBLANG_SINDHI_INDIA                        0x01    // Sindhi (India) reserved 0x0459
#define SUBLANG_SINDHI_PAKISTAN                     0x02    // Sindhi (Pakistan) reserved 0x0859
#define SUBLANG_SINDHI_AFGHANISTAN                  0x02    // For app compatibility only
#define SUBLANG_SINHALESE_SRI_LANKA                 0x01    // Sinhalese (Sri Lanka)
#define SUBLANG_SOTHO_NORTHERN_SOUTH_AFRICA         0x01    // Northern Sotho (South Africa)
#define SUBLANG_SLOVAK_SLOVAKIA                     0x01    // Slovak (Slovakia) 0x041b sk-SK
#define SUBLANG_SLOVENIAN_SLOVENIA                  0x01    // Slovenian (Slovenia) 0x0424 sl-SI
#define SUBLANG_SPANISH                             0x01    // Spanish (Castilian)
#define SUBLANG_SPANISH_MEXICAN                     0x02    // Spanish (Mexican)
#define SUBLANG_SPANISH_MODERN                      0x03    // Spanish (Modern)
#define SUBLANG_SPANISH_GUATEMALA                   0x04    // Spanish (Guatemala)
#define SUBLANG_SPANISH_COSTA_RICA                  0x05    // Spanish (Costa Rica)
#define SUBLANG_SPANISH_PANAMA                      0x06    // Spanish (Panama)
#define SUBLANG_SPANISH_DOMINICAN_REPUBLIC          0x07    // Spanish (Dominican Republic)
#define SUBLANG_SPANISH_VENEZUELA                   0x08    // Spanish (Venezuela)
#define SUBLANG_SPANISH_COLOMBIA                    0x09    // Spanish (Colombia)
#define SUBLANG_SPANISH_PERU                        0x0a    // Spanish (Peru)
#define SUBLANG_SPANISH_ARGENTINA                   0x0b    // Spanish (Argentina)
#define SUBLANG_SPANISH_ECUADOR                     0x0c    // Spanish (Ecuador)
#define SUBLANG_SPANISH_CHILE                       0x0d    // Spanish (Chile)
#define SUBLANG_SPANISH_URUGUAY                     0x0e    // Spanish (Uruguay)
#define SUBLANG_SPANISH_PARAGUAY                    0x0f    // Spanish (Paraguay)
#define SUBLANG_SPANISH_BOLIVIA                     0x10    // Spanish (Bolivia)
#define SUBLANG_SPANISH_EL_SALVADOR                 0x11    // Spanish (El Salvador)
#define SUBLANG_SPANISH_HONDURAS                    0x12    // Spanish (Honduras)
#define SUBLANG_SPANISH_NICARAGUA                   0x13    // Spanish (Nicaragua)
#define SUBLANG_SPANISH_PUERTO_RICO                 0x14    // Spanish (Puerto Rico)
#define SUBLANG_SPANISH_US                          0x15    // Spanish (United States)
#define SUBLANG_SWAHILI_KENYA                       0x01    // Swahili (Kenya) 0x0441 sw-KE
#define SUBLANG_SWEDISH                             0x01    // Swedish
#define SUBLANG_SWEDISH_FINLAND                     0x02    // Swedish (Finland)
#define SUBLANG_SYRIAC_SYRIA                        0x01    // Syriac (Syria) 0x045a syr-SY
#define SUBLANG_TAJIK_TAJIKISTAN                    0x01    // Tajik (Tajikistan) 0x0428 tg-TJ-Cyrl
#define SUBLANG_TAMAZIGHT_ALGERIA_LATIN             0x02    // Tamazight (Latin, Algeria) 0x085f tmz-DZ-Latn
#define SUBLANG_TAMIL_INDIA                         0x01    // Tamil (India)
#define SUBLANG_TATAR_RUSSIA                        0x01    // Tatar (Russia) 0x0444 tt-RU
#define SUBLANG_TELUGU_INDIA                        0x01    // Telugu (India (Telugu Script)) 0x044a te-IN
#define SUBLANG_THAI_THAILAND                       0x01    // Thai (Thailand) 0x041e th-TH
#define SUBLANG_TIBETAN_PRC                         0x01    // Tibetan (PRC)
#define SUBLANG_TIGRIGNA_ERITREA                    0x02    // Tigrigna (Eritrea)
#define SUBLANG_TSWANA_SOUTH_AFRICA                 0x01    // Setswana / Tswana (South Africa) 0x0432 tn-ZA
#define SUBLANG_TURKISH_TURKEY                      0x01    // Turkish (Turkey) 0x041f tr-TR
#define SUBLANG_TURKMEN_TURKMENISTAN                0x01    // Turkmen (Turkmenistan) 0x0442 tk-TM
#define SUBLANG_UIGHUR_PRC                          0x01    // Uighur (PRC) 0x0480 ug-CN
#define SUBLANG_UKRAINIAN_UKRAINE                   0x01    // Ukrainian (Ukraine) 0x0422 uk-UA
#define SUBLANG_UPPER_SORBIAN_GERMANY               0x01    // Upper Sorbian (Germany) 0x042e wen-DE
#define SUBLANG_URDU_PAKISTAN                       0x01    // Urdu (Pakistan)
#define SUBLANG_URDU_INDIA                          0x02    // Urdu (India)
#define SUBLANG_UZBEK_LATIN                         0x01    // Uzbek (Latin)
#define SUBLANG_UZBEK_CYRILLIC                      0x02    // Uzbek (Cyrillic)
#define SUBLANG_VIETNAMESE_VIETNAM                  0x01    // Vietnamese (Vietnam) 0x042a vi-VN
#define SUBLANG_WELSH_UNITED_KINGDOM                0x01    // Welsh (United Kingdom) 0x0452 cy-GB
#define SUBLANG_WOLOF_SENEGAL                       0x01    // Wolof (Senegal)
#define SUBLANG_XHOSA_SOUTH_AFRICA                  0x01    // isiXhosa / Xhosa (South Africa) 0x0434 xh-ZA
#define SUBLANG_YAKUT_RUSSIA                        0x01    // Yakut (Russia) 0x0485 sah-RU
#define SUBLANG_YI_PRC                              0x01    // Yi (PRC)) 0x0478
#define SUBLANG_YORUBA_NIGERIA                      0x01    // Yoruba (Nigeria) 046a yo-NG
#define SUBLANG_ZULU_SOUTH_AFRICA                   0x01    // isiZulu / Zulu (South Africa) 0x0435 zu-ZA




//
//  Sorting IDs.
//

#define SORT_DEFAULT                     0x0     // sorting default

#define SORT_INVARIANT_MATH              0x1     // Invariant (Mathematical Symbols)

#define SORT_JAPANESE_XJIS               0x0     // Japanese XJIS order
#define SORT_JAPANESE_UNICODE            0x1     // Japanese Unicode order (no longer supported)
#define SORT_JAPANESE_RADICALSTROKE      0x4     // Japanese radical/stroke order

#define SORT_CHINESE_BIG5                0x0     // Chinese BIG5 order
#define SORT_CHINESE_PRCP                0x0     // PRC Chinese Phonetic order
#define SORT_CHINESE_UNICODE             0x1     // Chinese Unicode order (no longer supported)
#define SORT_CHINESE_PRC                 0x2     // PRC Chinese Stroke Count order
#define SORT_CHINESE_BOPOMOFO            0x3     // Traditional Chinese Bopomofo order

#define SORT_KOREAN_KSC                  0x0     // Korean KSC order
#define SORT_KOREAN_UNICODE              0x1     // Korean Unicode order (no longer supported)

#define SORT_GERMAN_PHONE_BOOK           0x1     // German Phone Book order

#define SORT_HUNGARIAN_DEFAULT           0x0     // Hungarian Default order
#define SORT_HUNGARIAN_TECHNICAL         0x1     // Hungarian Technical order

#define SORT_GEORGIAN_TRADITIONAL        0x0     // Georgian Traditional order
#define SORT_GEORGIAN_MODERN             0x1     // Georgian Modern order

// end_r_winnt

//
//  A language ID is a 16 bit value which is the combination of a
//  primary language ID and a secondary language ID.  The bits are
//  allocated as follows:
//
//       +-----------------------+-------------------------+
//       |     Sublanguage ID    |   Primary Language ID   |
//       +-----------------------+-------------------------+
//        15                   10 9                       0   bit
//
//
//  Language ID creation/extraction macros:
//
//    MAKELANGID    - construct language id from a primary language id and
//                    a sublanguage id.
//    PRIMARYLANGID - extract primary language id from a language id.
//    SUBLANGID     - extract sublanguage id from a language id.
//

#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define PRIMARYLANGID(lgid)    ((WORD  )(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((WORD  )(lgid) >> 10)


//
//  A locale ID is a 32 bit value which is the combination of a
//  language ID, a sort ID, and a reserved area.  The bits are
//  allocated as follows:
//
//       +-------------+---------+-------------------------+
//       |   Reserved  | Sort ID |      Language ID        |
//       +-------------+---------+-------------------------+
//        31         20 19     16 15                      0   bit
//
//
//  Locale ID creation/extraction macros:
//
//    MAKELCID            - construct the locale id from a language id and a sort id.
//    MAKESORTLCID        - construct the locale id from a language id, sort id, and sort version.
//    LANGIDFROMLCID      - extract the language id from a locale id.
//    SORTIDFROMLCID      - extract the sort id from a locale id.
//    SORTVERSIONFROMLCID - extract the sort version from a locale id.
//

#define NLS_VALID_LOCALE_MASK  0x000fffff

#define MAKELCID(lgid, srtid)  ((DWORD)((((DWORD)((WORD  )(srtid))) << 16) |  \
                                         ((DWORD)((WORD  )(lgid)))))
#define MAKESORTLCID(lgid, srtid, ver)                                            \
                               ((DWORD)((MAKELCID(lgid, srtid)) |             \
                                    (((DWORD)((WORD  )(ver))) << 20)))
#define LANGIDFROMLCID(lcid)   ((WORD  )(lcid))
#define SORTIDFROMLCID(lcid)   ((WORD  )((((DWORD)(lcid)) >> 16) & 0xf))
#define SORTVERSIONFROMLCID(lcid)  ((WORD  )((((DWORD)(lcid)) >> 20) & 0xf))

// 8 characters for language
// 8 characters for region
// 64 characters for suffix (script)
// 2 characters for '-' separators
// 2 characters for prefix like "i-" or "x-"
// 1 null termination
#define LOCALE_NAME_MAX_LENGTH   85

//
//  Default System and User IDs for language and locale.
//

#define LANG_SYSTEM_DEFAULT    (MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))

#define LOCALE_SYSTEM_DEFAULT  (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))

//
//  Other special IDs for language and locale.
//
#define LOCALE_CUSTOM_DEFAULT                                                 \
          (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_CUSTOM_DEFAULT), SORT_DEFAULT))

#define LOCALE_CUSTOM_UNSPECIFIED                                             \
          (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_CUSTOM_UNSPECIFIED), SORT_DEFAULT))

#define LOCALE_CUSTOM_UI_DEFAULT                                              \
          (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_UI_CUSTOM_DEFAULT), SORT_DEFAULT))

#define LOCALE_NEUTRAL                                                        \
          (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT))

#define LOCALE_INVARIANT                                                      \
          (MAKELCID(MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL), SORT_DEFAULT))

*/
/*
class Path;

void swap(Path&, Path&);
bool lexicographical_compare(Path::iterator, Path::iterator,
							 Path::iterator, Path::iterator);

bool operator==(const Path&, const Path&);
bool operator!=(const Path&, const Path&);
bool operator<(const Path&, const Path&);
bool operator<=(const Path&, const Path&);
bool operator>(const Path&, const Path&);
bool operator>=(const Path&, const Path&);

path operator/(const Path&, const Path&);

std::ostream& operator<<(std::ostream&, const Path&);
std::wostream& operator<<(std::wostream&, const Path&);
std::istream& operator>>(std::istream&, Path&);
std::wistream& operator>>(std::wistream&, Path&)

class FileSystemError;
class DirectoryEntry;

class HDirectory;
class HRecursiveDirectory;

struct EFileType
{
	enum FileType
	{
		StatusError,
		NotFound,
		Regular,
		Directory,
	//	Symlink,
	//	Block,
		Character,
		FIFO,
	//	Socket,
		UnknownType
	};
};

struct FileStatus;

typedef uintmax_t FileSizeType;

struct SpaceInfo  //空间计算函数返回。
{
	FileSizeType Capacity;
	FileSizeType Free; 
	FileSizeType Available; //非特权进程可用空间。
};

struct ECopyOption
{
	enum //IfExists
	{
		Fail,
		Overwrite
	};
};


//操作。
bool IsDirectory(FileStatus);
bool IsDirectory(const Path&);
bool IsEmpty(const Path&);
bool IsKnownStatus(FileStatus);
bool IsOther(FileStatus);
bool IsOther(const Path&,);
bool IsRegularFile(FileStatus); 
bool IsRegularFile(const Path&);

bool BeEquivalent(const Path&, const Path&);

bool Exists(FileStatus);
bool Exists(const Path&);

Path			GetAbsolutePath(const Path&, const Path& = GetCurrentPath());
std::time_t		GetLastWriteTime(const Path&);
SpaceInfo		GetSpaceInfo(const Path&);
FileStatus		GetStatus(const Path&);
Path			GetCurrentPath();
void			GetCurrentPath(Path&);
FileSizeType	GetFileSize(const Path&);

void SetLastWriteTime(const Path&, const std::time_t);

bool CreateDirectory(const Path&);
bool CreateDirectories(const Path&);

void CopyFile(const Path&, const Path&);
void CopyFile(const Path&, const Path&, ECopyOption);

bool Remove(const Path&);

FileSizeType	RemoveAll(const Path&);

void			Rename(const Path&, const Path&);

Path SystemComplete(const Path&);
//Path		unique_path(const Path& model="%%%%-%%%%-%%%%-%%%%");
*/

typedef char NativePathCharType; //本地路径字符类型，POSIX 为 char ，Windows 为 wchar_t。

/*
struct utf8_codecvt_facet :
	public std::codecvt<wchar_t, char, std::mbstate_t>
{
public:
	explicit utf8_codecvt_facet(std::size_t no_locale_manage = 0)
		: std::codecvt<wchar_t, char, std::mbstate_t>(no_locale_manage)
	{}

protected:
	virtual std::codecvt_base::result do_in(
		std::mbstate_t& state, 
		const char* from,
		const char* from_end, 
		const char*& from_next,
		wchar_t* to, 
		wchar_t* to_end, 
		wchar_t*& to_next
		) const;

	virtual std::codecvt_base::result do_out(
		std::mbstate_t& state, const wchar_t* from,
		const wchar_t* from_end, const wchar_t*& from_next,
		char* to, char* to_end, char *& to_next
		) const;

	bool invalid_continuing_octet(unsigned char octet_1) const
	{
		return octet_1 < 0x80 || 0xbf < octet_1;
	}

	bool invalid_leading_octet(unsigned char octet_1) const
	{
		return (0x7F < octet_1 && octet_1 < 0xC0) || (octet_1 > 0xFD);
	}

	// continuing octets = octets except for the leading octet
	static unsigned int get_cont_octet_count(unsigned char lead_octet)
	{
		return get_octet_count(lead_octet) - 1;
	}

	static unsigned int get_octet_count(unsigned char lead_octet);

	// How many "continuing octets" will be needed for this word
	// ==   total octets - 1.
	int get_cont_octet_out_count(wchar_t word) const;

	virtual bool
	do_always_noconv() const throw()
	{
		return false;
	}

	// UTF-8 isn't really stateful since we rewind on partial conversions
	virtual std::codecvt_base::result do_unshift(
		std::mbstate_t&,
		char * from,
		char * ,//to,
		char * & next
		) const 
	{
		next = from;
		return ok;
	}

	virtual int do_encoding() const throw()
	{
		const int variable_byte_external_encoding=0;
		return variable_byte_external_encoding;
	}

	// How many char objects can I process to get <= max_limit
	// wchar_t objects?
	virtual int do_length(
		const std::mbstate_t&,
		const char* from,
		const char* from_end, 
		std::size_t max_limit
		) const;

	// Largest possible value do_length(state,from,from_end,1) could return.
	virtual int do_max_length() const throw ()
	{
		return 6; // largest UTF-8 encoding of a UCS-4 character
	}
};

class Path
{
public:
	typedef NativePathCharType value_type;
	typedef std::basic_string<value_type> StringType;
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

private:
	StringType Pathname;

public:
	//编码转换。
	static std::locale imbue(const std::locale&);
	static const codecvt_type& codecvt();

	// constructors and destructor
	Path();
	Path(const Path& p);
//	template<class _tSource>
//	Path(_tSource const&, const codecvt_type& = codecvt());
//	template<class _tInIt>
//	Path(_tInIt, _tInIt, const codecvt_type& = codecvt());
	~Path();

	//赋值。
	Path&
	operator=(const Path& p);
//	template<class _tSource>
//	Path&
//	operator=(const _tSource& source);

//	template<class _tSource>
//	Path&
//	assign(_tSource const& source, const codecvt_type&)
//	template<class _tInIt>
//	Path&
//	assign(_tInIt, _tInIt, const codecvt_type& = codecvt());


	//追加路径。
	Path&
	operator/=(const Path& p);
//	template<class _tSource>
//	Path&
//	operator/=(const _tSource&);

//	template<class _tSource>
//	Path&
//	append(const _tSource&, const codecvt_type&);
//	template<class _tInIt>
//	Path&
//	append(_tInIt begin, _tInIt end, const codecvt_type& = codecvt());

	//modifiers
	void clear();
	void swap(Path& rhs) ythrow();

	Path& MakeAbsolute(const path& base);
	Path& MakePreferred(); // POSIX: no effect. Windows: convert slashes to backslashes
	Path& RemoveFilename();
	Path& ReplaceExtension(const path& new_extension = path());

	const string_type&
	native() const; //本地格式和编码。
	const value_type*
	c_str() const; //本地格式和编码的 C 风格字符串。

	template<class _tString>
	_tString GetString(const codecvt_type& = codecvt()) const; //本地字符串格式。

//	const string    string(const codecvt_type& = codecvt()) const; // native format
//	const wstring   wstring(const codecvt_type& = codecvt()) const; // ditto
//	const u16string u16string() const; // ditto
//	const u32string u32string() const; // ditto

	template<class _tString>
	_tString GetGenericString() const;
//	const string    GetGenericString(const codecvt_type& cvt=codecvt()) const;   // generic format
//	const wstring   GetGenericString(const codecvt_type& cvt=codecvt()) const;  // ditto
//	const u16string GetGenericString() const;                                 // ditto
//	const u32string GetGenericString() const;                                 // ditto

	//查询。
	bool IsEmpty() const;
	bool IsAbsolute() const;
	bool IsRelative() const;
	bool HasRootName() const;
	bool HasRootDirectory() const;
	bool HasRootPath() const;
	bool HasRelativePath() const;
	bool HasParentPath() const;
	bool HasFilename() const;
	bool HasStem() const;
	bool HasExtension() const;

	//路径分解。
	Path GetRootName() const;
	Path GetRootDirectory() const;
	Path GetRootPath() const;
	Path GetRelativePath() const;
	Path GetParentPath() const;
	Path GetFilename() const;
	Path GetStem() const;
	Path GetExtension() const;

	//迭代器。
	class iterator;
	typedef iterator const_iterator;

	iterator begin() const;
	iterator end() const;
};
*/
//	lblC.RequestFocus();
//	lblC.ReleaseFocus();

//	lblC.Text = L"测试= =";
//	HWND h(lblC.GetWindowHandle());
//	lblC.Text[0] ^= 1;
//	String::iterator i(lblC.Text.begin());
//	++*i;
//	tf.SetPos(0);
//	tf.Seek(-128, SEEK_END);
//	sprintf(strtf, "%d,%d,%d,%d,%d", tf.IsValid(), tf.GetBOMLen(), tf.GetLen(), tf.GetCP(), tf.GetPos());

/*
struct non_inheritable
{
};
static void
_testStr(const String& s)
{
	char* str = sdup(str.c_str(), '?');
	yprintf("%s\n",x,str);
	free(str);
}
*/
/*
static void
RectDarken(BitmapPtr src, const SPoint& l, const SSize& s, HWND hWnd)
{
	YAssert(hWnd, "err: hWnd is null.");

	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), SRect(l, s), s, transPixelEx, transSeq());
//	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), SRect(l, s), transPixelShiftRight(1), transSeq());


	IWidget* pWgt(dynamic_cast<IWidget*>(hWnd->GetFocusingPtr()));

	if(pWgt)
		DrawWidgetBounds(*pWgt, ARGB16(1, 31, 1, 31));

	GraphicInterfaceContext g(hWnd->GetBufferPtr(), hWnd->GetBounds());
//	DrawLineSeg(g, 2, 4, hWnd->GetWidth(), hWnd->GetHeight(), PixelType(ARGB16(1, 9, 4, 31)));
//	DrawLineSeg(g, 2, 10, 12, 65, PixelType(ARGB16(1, 1, 31, 1)));

	DrawWindowBounds(hWnd, ARGB16(1, 1, 1, 31));
//	DrawHLineSeg(g, 1, 10, 65, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawVLineSeg(g, 1, 10, 65, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawPoint(g, 3, 3, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawRect(g, 11, 12, 19, 24, PixelType(ARGB16(1, 4, 31, 31)));
//	DrawWndBounds(hWnd, ARGB16(1, 4, 31, 9));
	DrawLineSeg(g, 2, 3, 22, 9, PixelType(ARGB16(1, 31, 0, 0)));
	DrawLineSeg(g, 22, 3, 42, 28, PixelType(ARGB16(1, 31, 15, 0)));
	DrawLineSeg(g, 52, 29, 62, 3, PixelType(ARGB16(1, 31, 31, 0)));
	DrawLineSeg(g, 72, 11, 82, 3, PixelType(ARGB16(1, 0, 31, 0)));
	DrawLineSeg(g, 102, 5, 82, 13, PixelType(ARGB16(1, 0, 0, 31)));
	DrawLineSeg(g, 122, 3, 102, 35, PixelType(ARGB16(1, 11, 0, 31)));
	DrawLineSeg(g, 122, 9, 142, 1, PixelType(ARGB16(1, 31, 0, 31)));
	DrawLineSeg(g, 132, 35, 152, 3, PixelType(ARGB16(1, 31, 21, 31)));
}
*/
/*
sprintf(strtf, "B%p,%p,%p,%p,%p\n", !!pDefaultFontCache->GetTypefacePtr("FZYaoTi", "Regular"),
!!pDefaultFontCache->GetTypefacePtr("FZYaoTi", "Bold"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Regular"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Bold"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Italic")
);

sprintf(strtf, "A%p,%p,%p\n", pDefaultFontCache->GetFontFamilyPtr("FZYaoTi"),
pDefaultFontCache->GetFontFamilyPtr("YouYuan"),
pDefaultFontCache->GetFontFamilyPtr("Microsoft YaHei")
);
*/
/*
#include "../YSLib/Core/ystring.h"

class Path
{
public:
	typedef String PathType;
	typedef PathType& RefType;
	typedef const PathType& CRefType;

protected:
	PathType Directory;
	PathType File;

public:
	Path(CRefType);

	CRefType GetDirectory() const;
	CRefType GetFile() const;

	void SetPath(CRefType);
};

inline Path::Path(Path::CRefType path)
{
	SetPath(path);
}

inline Path::CRefType Path::GetDirectory() const
{
	return Directory;
}
inline Path::CRefType Path::GetFile() const
{
	return File;
}

inline void Path::SetPath(Path::CRefType path)
{
	SplitPath(path, Directory, File);
}


YClass YFileItem
{
private:
	DIR_ITER* dir;
	stat st;
	PATHSTR fileName;

public:
	~YFileItem();

	bool IsDirectory() const;
	bool IsValid() const;

	void Open(CPATH path);
	void Reset();
	void Close();
	int ToNext();
};

inline YFileItem::~YFileItem()
{
	Close();
}

inline bool YFileItem::IsDirectory() const
{
	return st.st_mode & S_IFDIR;
}
inline bool YFileItem::IsValid() const
{
	return dir != NULL;
}

void YFileItem::Open(CPATH path)
{
	dir = diropen(path);
}
void YFileItem::Reset()
{
	dirreset(dir);
}
void YFileItem::Close()
{
	if(dir)7
		dirclose(dir);
}
int YFileItem::ToNext()
{
	return dirnext(dir, fileName, &st);
}*/


/*
class YListControl : public YObject
{
protected:
	Path alloc;

public:
	YListControl();
};

YFileListControl(CPATH path)
{
}

*/

//unverified:

/*


class YLabeledWidget : public GMCounter<LabeledWidget>, public YWidget
{
protected:
	YLabel& label;
	YWidget& widget;

public:
	template<class _CharT>
		YLabeledWidget(HWND hWnd, const _CharT*, YWidget*, const Font::SizeType = Font::DefSize);
	virtual ~LabeledWidget();

	virtual void SetBounds(s16 x, s16 y, u16 w, u16 h);
	virtual void SetBounds(SRect r);

};
void YLabeledWidget::SetBounds(SRect& r)
{
	YWidget::SetBounds(r);

	label.SetBounds(r.x, r.y, r.w >> 1, r.h);
	widget.SetBounds(r.x + (r.w>>1), r.y, r.w - (r.w >> 1), r.h);
}
*/

/*
//	(prTextRegion->*SelectRenderer(Transparent))
class YCommandButton : public GMCounter<YCommandButton>, public YWidget //命令按钮。
{
public:
	typedef YWidget ParentType;
protected:
	YLabel* label;
	YImage* image;

public:
	String& Caption;
	u8& FontSize;

	YCommandButton();
};


*/
	/*
	//设置主屏显示模式为帧缓冲，副屏不使用
	videoSetMode(MODE_FB0);
	videoSetModeSub(0);
	vramSetBankA(VRAM_A_LCD);
	for(u16 y = 60; y < SCRH - 60; ++y)
		for(u16 x = 60; x < SCRW - 60; ++x)
			VRAM_A[y * SCRW + x] = RGB15((x+5)%31,y%31,(x&y)&31);
	PixelType color = RGB15(31,0,0)|BITALPHA;
	*/

	//对象测试段。
	//YImage* pyi = new YImage;
	//sprintf(strtbuf + strlen(strtbuf), "a.addr>%p,pyi>size%d,addr:0x%x;\n",&theApp,&a,sizeof(*pyi),pyi);

	/*

	*/
	/*
	shltxt.PrintString("CaoNiMaYahoo爷们万碎！！！wwwwwwwww");
	uchar_t ustr[256];
	wchar_t wstr[512];
	u32 ucl = MBCSToUTF16LE(ustr, str, GBK);
	w32 wcl = MBCSToUCS(wstr, str, GBK);
	shltxt.PutString(ustr);
	shltxt.PutString(wstr);
	shltxt.PutString(L"Innovation in China.!?<%^&*>");
	*/
	//shltxt.CopyToBuffer(pDesktopUp->GetPtr());
	//shltxt.BlitToBuffer(pDesktopUp->GetPtr());
//	for(int i = 0;i < 128; ++i)
	/*
	sprintf(strttxt + strlen(strttxt), "%x,", shltxt.GetBufferPtr()[i]);
	MDualScreenReader& dsr = *pdsr;
	TextRegion& shltxt = dsr.GetUp();
	sprintf(strtbuf + strlen(strtbuf), "buffer addr : 0x%x", (int)shltxt.GetBufferPtr());
	sprintf(strtmargin, "%d, %d, %d, %d;", shltxt.Margin.GetLeft(), shltxt.Margin.GetRight(), shltxt.Margin.GetTop(), shltxt.Margin.GetBottom());
	sprintf(strtf,"xxx:%u,%u;%u,%u\n",shltxt.Margin.GetBottom(),shltxt.ResizeMargin(),shltxt.GetPenX(),shltxt.GetPenY());
	sprintf(strtpen, "penX = %d, penY = %d;", shltxt.GetPenX(), shltxt.GetPenY());
	*/

//AGG Drawing Test

/*
static u8* frameBuffer = new u8[sizeof(ScreenBufferType)];
static agg::rendering_buffer& grbuf = *new agg::rendering_buffer(frameBuffer, SCRW, SCRH, SCRW * sizeof(PixelType));

typedef agg::renderer_base<agg::pixfmt_argb16> rend_type;
static rend_type rend;//(grbuf);
static agg::rasterizer_outline<rend_type> rast(rend);*/

////

/*
static void
ad_rect(const SRect& r, const agg::rgba8* c)
{
	rast.move_to(r.x, r.y);
	rast.line_to(r.x + r.w, r.y);
	rast.line_to(r.x + r.w, r.y + r.h);
	rast.line_to(r.x, r.y + r.h);
	rast.line_to(r.x, r.y);
	if(c)
		rast.render(rend, *c);
}
typedef std::vector<SPoint> poly; //多边形。
static void
ad_v(const poly& p, const agg::rgba8* c)
{
	poly::const_iterator i = p.begin();

	rast.move_to(i->x, i->y);
	while(++i != p.end())
		rast.line_to(i->x, i->y);
	if(c)
		rast.render(rend, *c);
}
*/
/*
static void on_draw()
{
	agg::rendering_buffer &rbuf = grbuf;
	agg::pixfmt_rgb555 pixf(rbuf);

	typedef agg::renderer_base<agg::pixfmt_rgb555> renderer_base_type;
	renderer_base_type renb(pixf);

	typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	renderer_scanline_type rensl(renb);

	// Vertex Source
	agg::ellipse ell(100,100,50,50); //圆心在中间
	// Coordinate conversion pipeline

	agg::trans_affine mtx;
	mtx.scale(0.5,1); // x轴缩小到原来的一半
	mtx.rotate(agg::deg2rad(30)); // 旋转30度
	mtx.translate(100,100); // 平移100,100
	typedef agg::conv_transform<agg::ellipse> ell_ct_type;
	ell_ct_type ctell(ell, mtx); // 矩阵变换
	typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	ell_cc_type ccell(ctell); // 轮廓变换

	typedef agg::conv_contour<agg::ellipse> ell_cc_type;
	ell_cc_type ccell(ell);
	typedef agg::conv_stroke<ell_cc_type> ell_cc_cs_type;
	ell_cc_cs_type csccell(ccell); // 转换成多义线

	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_u8 sl;

	renb.clear(rgb8(255, 255, 255));

//	renb.clip_box(30, 30, 160, 160); // 设置可写区域


	for(int i = 0; i < 5; ++i)
	{
		ccell.width(i * 20);
		ras.add_path(csccell);
		rensl.color(rgb8(0, 0, i * 50));
		agg::render_scanlines(ras,sl,rensl);
	}
	agg::arrowhead ah;
	ah.head(0, 10, 5, 5);
	ah.tail(10, 10, 5, 5);
	// 用path_storage生成一条直线
	agg::path_storage ps;
	ps.move_to(160, 60);
	ps.line_to(100, 100);
	// 转换
	agg::conv_stroke<agg::path_storage, agg::vcgen_markers_term> csps(ps);
	agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead>
		arrow(csps.markers(), ah);
	// 画线
	ras.add_path(csps);
	agg::render_scanlines_aa_solid(ras, sl, renb, rgb8(0, 0, 0));
	// 画箭头
	ras.add_path(arrow);
	agg::render_scanlines_aa_solid(ras, sl, renb, argb16(1, 31, 0, 0));

	//从50,20开始，画20条长度为100的坚线，颜色从黑渐变到红，覆盖率为128(半透明)
	for(int i = 0; i < 31; ++i)
		pixf.blend_vline(50 + i*2, 20, 100, rgb5(i, 0, 0), 128);


	agg::int8u* p = rbuf.row_ptr(20); //得到第20行指针
	memset(p, 0, rbuf.stride_abs()); //整行以0 填充


}
*/
/*
static inline void aggcopy(BitmapPtr dst, agg::rendering_buffer& src)
{
	scrCopy(dst, (ConstBitmapPtr)src.buf());
}
static void aggtest()
{
	on_draw();
	aggcopy(pDesktopUp->GetPtr(), grbuf);
}
*/

/*
//基类同名函数映射。

#ifdef __GNUC__


#	define DefineFuncToBase(_name, _base, _type, _paralist...) _type _name(__MakeParaList(##_paralist)) { return _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseVoid(_name, _base, _paralist...) void _name(__MakeParaList(##_paralist)) { _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseConst(_name, _base, _type, _paralist...) _type _name(__MakeParaList(##_paralist)) const { return _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseVoidConst(_name, _base, _paralist...) void _name(__MakeParaList(##_paralist)) const { _base::_name(__MakeArgList(##_paralist)); }
#	define __MakeParaList(_type, _name, _other...) _type _name, __MakeParaList(##_other)
#	define __MakeArgList(_type, _name, _other...) _name, __MakeArgList(##_other)
#	define __MakeParaList()
#	define __MakeArgList()

#else

#	define DefineFuncToBase(_name, _base, _type, _para, ...) _type _name {} { return _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseVoid(_name, _base, _para, ...) void _name {} { _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseConst(_name, _base, _type, _para, ...) _type _name {} const { return _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseVoidConst(_name, _base, _para, ...) void _name {} const { _base::_name(_para, ##__VA_ARGS__); }

#endif
*/
/*
fullw
ARGB16(1,31,31,31);

static void dfrandom(BitmapPtr buf, u16 x, u16 y)
{
	buf[y * SCRW + x] = rand() & 0xffff;
}

m1
	ARGB16(((x*3+y*5)|(x*5+y*7))&1,(x+5)%31,y%31,(x&y)&31);

s1
	ARGB16(1, (x + 5) & 31, y & 31, (x & y) & 31);
t1
	ARGB16(1, (x + 2) % 14 + 13, y % 7 + 19, (x >> (y & 1)) % 11 + 6);
t2
	ARGB16(1, (x + 1) % 24 + 13, y % 17 + 9, (x >> (y & 1)) % 21 + 6);
b1
	ARGB16(1, (x + 5) % 23 + 4, y % 21 + 3, (x ^ y) % 22 + 7);
c1
	ARGB16(1, (x + 5) % 21 + 2, y % 23 + 4, (x & y) % 22 + 7);
ry1
	ARGB16(1, rand() & 31, rand() & 31, rand() & 15 & (rand() & 1) << 4);
ar1
	ARGB16(1, ((x + 5) % 83 + 4) & 31, (y % 71 + 3) & 31, (x ^ ~y) % 22 + 9);
ar2
	ARGB16(1, ((x + 5) % 89 + 4) & 31, (y % 23 + 3) & 31, (x ^ ~y) % 37 - 9);
u1
	ARGB16(1, ((x + 27) % 53 + 4) & 31, (~y % 233 + 3) & 31, (x * y) & 31);
v1
	ARGB16(1, ((x + y) % ((y - 2) & 1) + (x << 2)) & 31, (~x % 101 + y) & 31, ((x << 4) / (y & 1)) & 31);
yu1
	ARGB16(1, (y % x) & 31, (x % y) & 31, (x * y) & 31);
arz1
	ARGB16(1, ((x | y) % (y + 2)) & 31, ((~y | x) % 27 + 3) & 31, ((x << 6) / (y | 1)) & 31);
f1
	ARGB16(1, (x + y) & 31, (x - y) & 31, (x | y) & (x ^ y) & 31);
fl1
	ARGB16(1, (x + y * y) & 31, (x * x + y) & 31, ((x & y) ^ (x | y)) & 31);
fb1
	ARGB16(1, ((x + y) | y) & 31, ((x + y) | x) & 31, ((x ^ y) & (x ^ y)) & 31);
fb2
	ARGB16(1, ((x + y) & y) & 31, ((x + y) & x) & 31, ((x & y) ^ (x | y)) & 31);
rs1
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, ((x ^ y) + (x ^ y)) & 31);
rs2
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, ((x & y) * (x | y)) & 31);
rs3
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, ((x ^ y) * (x ^ y)) & 31);
rs4
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, ((x & y) + (x | y)) & 31);
rs5
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, (x * y) & 31);
rs6
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, (x + y) & 31);
rs7
	ARGB16(1, ((x * y) | ~y) & 31, ((x * y) | ~x) & 31, ~(x * y) & 31);
rs8
	ARGB16(1, ((x * y) & ~y) & 31, ((x * y) & ~x) & 31, ~(x + y) & 31);
rs9
	ARGB16(1, (x + y) & 31, (x * y) & 31, (x - y) & 31);
ra1
	ARGB16(1, x & 31, y & 31, ~(x * y) & 31);
ra2
	ARGB16(1, ~y & 31, ~x & 31, (~x * ~y) & 31);
raz1
	ARGB16(1, (x << 2) + 15 & 31, (y << 2) + 15 & 31, (~(x * y) << 2) + 15 & 31);
raz2
	ARGB16(1, ((x >> 2) + 15) & 31, ((y >> 2) + 15) & 31, ((~(x * y) >> 2) + 15) & 31);
bza1
	ARGB16(1, ((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31, ((x << 4) / (y | 1)) & 31);
*/
