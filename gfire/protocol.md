<style>
  h1 {
    color: #005A9C;
  
    /* Add 1 to chapter */
    counter-increment: chapter;

    /* Set section to 0 */
    counter-reset: section 0;
  }

  h1:before {
    content: counter(chapter) ". ";
  }

  h2 {
    color: #005A9C;
  
    /* Add 1 to chapter */
    counter-increment: section;

    /* Set section to 0 */
    counter-reset: subsection 0;
  }

  h2:before {
    content: counter(chapter) "." counter(section) " ";
  }

  h3 {
    color: #005A9C;
  }
</style>

# Open Connection

1. open connection to cs.xfire.com and port 25999
2. send OC1 to cs.xfire.com
3. send Packet 18 to cs.xfire.com
4. os.xfire.com sends AFA
5. send AUTH1 to cs.xfire.com
6. os.xfire.com sends ASUC
7. send CONF

# Information

* SID = 16 bytes long
* salt = 40 byte SHA1 thing
* userid = 4 bytes long

# Password

When we have a password null and username null we receive this hash in the config:

```
5853ab14df6d90744943734beee321f09b5eaf41
```

this hash comes from this:

_UltimateArena_

the hash in the config is this:

```
sha1(username + password + UltimateArena)
```

we send the password with this:

```
sha1(sha1(username + password + UltimateArena) + salt)
```

# Packets

Every packet has a packet header. Its constructed in the following way.

X1 X2 YY 00 ZZ

X1 + X2 = packet length = X1 + X2*256

YY = packet ID

00 = unknown, perhaps the same thing as packet length for packet ID

ZZ = number of attributes that are in that packet.

# Authentication (Client Side)

## Open Connection

### Description

This Packet opens the connection to the server.

### Content

It only contains a String:

```
UA01
```

### Example

```
00000000  55 41 30 31                                       UA01
```

## Packet 18

### Description

We send Basic information about our client.

### Content

1. the skins we use
2. the version of each skin
3. the protocol version

* `magicNumber 00 type(18) 00 numberOfAtts //skins and versions of skins are 1`
* `attribute_length 'skin' 04 01 numberOfSkins 00 lengthOfNextSkin 00 nameOfSkin LengthOfNextSkin 00 nameOfSkin (..andSoOn..)`
* `attribute_length 'version' 04 02 numberOfSkins 00 (forEachSkin){ 01 00 00 00 } 12 00 03 00 01`
* `attribute_length 'version' 02 versionNumber 00 00 00`

### Example

```
00000004  33 00 12 00 02 04 73 6B  69 6E 04 01 02 00 08 00  3.....sk in......
00000014  53 74 61 6E 64 61 72 64  05 00 58 66 69 72 65 07  Standard ..Xfire.
00000024  76 65 72 73 69 6F 6E 04  02 02 00 01 00 00 00 01  version. ........
00000034  00 00 00 12 00 03 00 01  07 76 65 72 73 69 6F 6E  ........ .version
00000044  02 19 00 00 00                                    .....
```

## Packet 1

### Description

We send our username and password, which is encrypted, to the server

### Content

1. the username
2. the encrypted password
    * `Password = sha1(sha1(username + password + "UltimateArena") + salt)`

* `packet_length 00 type(01) 00 numberOfAtts`
* `attribute_length 'name' usernameLength_length usernameLength 00 username`
* `attribute_length 'password' passwdLength_length passwdLength 00 cryptedPassword`

### Example

```
00000049  49 00 01 00 02 04 6E 61  6D 65 01 08 00 61 73 72  I.....na me...asr
00000059  61 6E 69 65 6C 08 70 61  73 73 77 6F 72 64 01 28  aniel.pa ssword.(
00000069  00 38 30 30 31 34 31 65  36 35 30 31 36 32 30 66  .800141e 6501620f
00000079  61 39 30 34 65 31 61 66  62 66 61 36 31 66 30 63  a904e1af bfa61f0c
00000089  34 65 31 33 35 38 64 66  65                       4e1358df e
```

## Packet 4

### Description

Join a game

### Content

* `packet_length 00 type(4) 00 numberOfAtts`
* `attribute_length 'gameid' 02 gameid`

### Example

```
00000443  11 00 04 00 01 06 67 61  6D 65 69 64 02 00 00 00  ......ga meid....
00000453  00                                                .
```

## Packet 6

### Description

Invite a buddy

### Content

* `packet_length 00 type(6) 00 numberOfAtts`
* `attribute_length 'name' number_of_names name_length 00 name`
* `attribute_length 'msg' number_of_msgs msg_length 00 msg`

### Example

```
0000020A  33 00 06 00 02 04 6E 61  6D 65 01 06 00 6B 65 61  3.....na me...kea
0000021A  6D 6F 73 03 6D 73 67 01  19 00 43 75 73 74 6F 6D  mos.msg. ..Custom
0000022A  20 69 6E 76 69 74 61 74  69 6F 6E 20 6D 65 73 73   invitat ion mess
0000023A  61 67 65                                          age
```

## Packet 7

### Description

Accept invitation

### Content

* `packet_length 00 packet_type(07) 00 numberOfAtts`
* `attribute_length 'name' 01 name_length 00 name`

### Example

```
00000000  15 00 07 00 01 04 6E 61  6D 65 01 08 00 61 73 72  ......na me...asr
00000010  61 6E 69 65 6C                                    aniel
```

## Packet 9

### Description

Remove this buddy

### Content

* `packet_length 00 type(09) 00 numberOfAtts`
* `attribute_length 'userid' 02 userid`

### Example

```
000001CB  11 00 09 00 01 06 75 73  65 72 69 64 02 AB 3C 02  ......us erid..<.
000001DB  00                                                .
```

## Packet 11

### Description

Set status

### Content

* `(nextSegmentLength 00 0b 00 numberOfAtts-1)`
* `attribute_length 'type' 02 01 00 00 00`
* `attribute_length 'status' 05 01`
* `attribute_length 't' statusLength_length statusLength 00 status`

<br />

* `(nextSegmentLength 00 0b 00 numberOfAtts+1)`
* `attribute_length 'type' 02 00 00 00 00`

### Example

Go away

```
00000000  35 00 0B 00 02 04 74 79  70 65 02 01 00 00 00 06  5.....ty pe......
00000010  73 74 61 74 75 73 05 01  01 74 01 18 00 62 75 61  status.. .t...bua
00000020  68 61 68 61 2C 20 73 65  67 66 61 75 6C 74 20 68  haha, se gfault h
00000030  65 61 76 65 6E                                    eaven
```

Come back online

```
00000035  0F 00 0B 00 02 04 74 79  70 65 02 00 00 00 00     ......ty pe.....
```

## Packet 12

### Description

Ask for more information about a buddy

### Content

* `packet_length 00 type(12) 00 numberOfAtts`
* `attribute_length 'name' number_of_names name_length 00 name`
* `attribute_length 'fname' number_of_fnames fname_length 00 fname //first name`
* `attribute_length 'lname' number_of_lnames lname_length 00 lname //last name`
* `attribute_length 'email' number_of_emails email_length 00 email`

### Example

```
000001DC  2E 00 0C 00 04 04 6E 61  6D 65 01 06 00 6B 65 61  ......na me...kea
000001EC  6D 6F 73 05 66 6E 61 6D  65 01 00 00 05 6C 6E 61  mos.fnam e....lna
000001FC  6D 65 01 00 00 05 65 6D  61 69 6C 01 00 00        me....em ail...
```

## Packet 14

### Description

Alias change

### Content

* `packet_length 00 type(12) 00 numberOfAtts`
* `attribute_length 'nick' nickLength_length nickLength 00 nick`

### Example

```
000003B2  1B 00 0E 00 01 04 6E 69  63 6B 01 0E 00 43 68 61  ......ni ck...Cha
000003C2  6E 67 69 6E 67 20 6E 69  63 6B 73                 nging ni cks
```

## Packet 16

### Description

We send some information about us to the server

* language used
* skin used
* theme used
* partner

### Content

* `packet_length 00 packet_type(16) 00 numberOfAtts`
* `attribute_length 'lang' langLength_length langLength 00 'de'`
* `attribute_length 'skin' skinLength_length skinLength 00 SkinName`
* `attribute_length 'theme' themeLength_length themeLength 00 theme`
* `attribute_length 'partner' 01 00 00`

### Example

```
000000B5  37 00 10 00 04 04 6C 61  6E 67 01 02 00 75 73 04  7.....la ng...us.
000000C5  73 6B 69 6E 01 04 00 53  43 43 54 05 74 68 65 6D  skin...S CCT.them
000000D5  65 01 08 00 52 75 62 79  20 52 65 64 07 70 61 72  e...Ruby Red.par
000000E5  74 6E 65 72 01 00 00                              tner...
```

## Packet 32

### Description

Set Away message

### Content

* `packet_length 00 packet_type(16) 00 numberOfAtts`
* `2e 01 07 msg_length msg`

### Example

```
00000000  10 00 20 00 01 2E 01 07  00 61 73 64 66 64 73 66  .. ..... .asdfdsf
```

# Authentication (Server Side)

## Packet 128

### Description

With this packet the server asks for Authentication. We answer with packet number 1

* the skins we use
* the version of each skin
* the protocol version

### Content

* `packet_length 00 type(80) 00 numberOfAtts`
* `attribute_length 'salt' saltLength_length saltLength 00 salt`

### Example

```
00000000  35 00 80 00 01 04 73 61  6C 74 01 28 00 66 62 36  5.....sa lt.(.fb6
00000010  62 39 34 64 38 31 38 39  65 64 31 36 32 39 35 31  b94d8189 ed162951
00000020  32 35 34 66 34 64 32 38  39 38 30 31 34 63 66 37  254f4d28 98014cf7
00000030  66 65 65 63 64                                    feecd
```

## Packet 129

### Description

This error tells us that we don't have a valid username and password

* the reason for the connection refusing

### Content

* `packet_length 00 packet_type(129) 00 numberOfAtts`
* `attribute_length 'reason' 02 00 00 00 00`

### Example

```
00000035  11 00 81 00 01 06 72 65  61 73 6F 6E 00           ......re ason..
```

## Packet 130

### Description

We get this package as soon as we are logged

### Content

* `length_Of_packet 00 packet_type(130) 00 numberOfAtts`
* `attribute_length 'userid' 02 userid 00`
* `attribute_length 'sid' 03 sid(length = 16)`
* `attribute_length 'nick' nickLength_length nickLength 00 nick(can be 00 if no nick is set)`
* `attribute_length 'status' 02 03 00 00 00`
* `attribute_length 'dlset' 01 00 00`

### Example

```
00000035  43 00 82 00 05 06 75 73 65  72 69 64 02 82 9C 09  C.....us erid....
00000045  00 03 73 69 64 03 E3 F5 63  C0 8A 9C F8 1B 55 79  ..sid... c.....Uy
00000055  DD F8 D8 11 E3 14 04 6E 69  63 6B 01 00 00 06 73  .......n ick....s
00000065  74 61 74 75 73 02 03 00 00  00 05 64 6C 73 65 74  tatus... ...dlset
00000075  01 00 00
```

## Packet 131

### Description

Contact List, usernames, and their nicks

### Content

* `(nextSegmentLength type(131) 00 numberOfAtts)`
* `attribute_length 'friends' 04 01 numberOfFriends 00 userLength 00 userName userLength 00 userName`
* `attribute_length 'nick' 04 01 02 00 nickLength 00 nick nickLength 00 nick`
* `attribute_length 'userid' 04 02 02 00 userID userID`

### Example

```
00000035  49 00 82 00 05 06 75 73 65  72 69 64 02 AB 3C 02  I.....us erid..<.
00000045  00 03 73 69 64 03 1F 31 A4  91 E2 2A 7B 19 E0 00  ..sid..1 ...*{...
00000055  4D 9E 7A 58 E8 6B 04 6E 69  63 6B 01 06 00 4B 65  M.zX.k.n ick...Ke
00000065  61 6D 6F 73 06 73 74 61 74  75 73 02 03 00 00 00  amos.sta tus.....
00000075  05 64 6C 73 65 74 01 00 00                        .dlset.. .
```

## Packet 132

### Description

Contact List, buddys online

### Content

* `(nextSegmentLength 00 type(132) 00 numberOfAtts)`
* `attribute_length 'userid' 04 02 01 00 ab 3c 02 00`
* `attribute_length 'sid' 04 03 01 00 sid(16bytes)`

### Example

```
000000D2  XX XX XX XX XX XX XX XX XX  XX XX XX 2C 00 84 00  XXXXXXXX XXXX,...
000000E2  02 06 75 73 65 72 69 64 04  02 01 00 AB 3C 02 00  ..userid .....<..
000000F2  03 73 69 64 04 03 01 00 8A  AB D5 34 62 16 27 D7  .sid.... ...4b.'.
00000102  0A EE 01 DE DD 5B FD 95                           .....[..
```

```
000003C8  00 84 00 02 06 75 73 65  72 69 64 04 02 07 00 D6  .....use rid.....
000003D8  33 01 00 AB 3C 02 00 82  9C 09 00 09 30 0A 00 53  3...<... ....0..S
000003E8  06 0C 00 09 01 0D 00 8C  B2 12 00 03 73 69 64 04  ........ ....sid.
000003F8  03 07 00 22 EA 0F A1 FC  03 9E BF 5A 01 A5 8E 32  ...".... ...Z...2
00000408  8D 9E 8C 08 C7 32 B9 61  00 70 0E 3C AD 46 79 C7  .....2.a .p.<.Fy.
00000418  A3 D9 AD 92 27 41 6C 35  BE AA F6 76 EA B0 CD E5  ....'Al5 ...v....
00000428  5E 27 79 27 58 68 CF 55  F7 DE E9 B8 95 CA 1C 38  ^'y'Xh.U .......8
00000438  A2 3A 6B 73 C7 D9 6E D6  91 01 E9 C1 EC 50 BF FC  .:ks..n. .....P..
00000448  09 58 4A AA 5F B2 5B 0C  75 A1 DE F6 79 44 F3 E6  .XJ._.[. u...yD..
00000458  E4 FB 61 23 B7 5D 8D 20  98 CB 0B 28 4F 9B 81 F5  ..a#.].  ...(O...
00000468  EA 4B 5C                                          ...
```

```
0000003E  2C 00 84 00 02 06 75 73  65 72 69 64 04 02 01 00  ,.....us erid....
0000004E  82 9C 09 00 03 73 69 64  04 03 01 00 00 00 00 00  .....sid ........
0000005E  00 00 00 00 00 00 00 00  00 00 00 00              ........ ....
```

## Packet 134

### Description

auth rejected, version too old

* the reason for the connection refusing

### Content

* `packet_length 00 packet_type(86) 00 numberOfAtts`
* `attribute_length 'version' 04 02 01 00 wantedVersionNumber 00 00 00`
* `attribute_length 'file' 04 01 01 00 urlLength 00 url`
* `attribute_length 'command' 04 02 01 00 01 00 00 00`
* `attribute_length 'fileid' 04 02 01 00 00 00 00 00`

### Example

```
00000035  5A 00 86 00 04 07 76 65  72 73 69 6F 6E 04 02 01  Z.....ve rsion...
00000045  00 23 00 00 00 04 66 69  6C 65 04 01 01 00 1B 00  .#....fi le......
00000055  68 74 74 70 3A 2F 2F 36  34 2E 35 36 2E 31 39 39  http://6 4.56.199
00000065  2E 31 33 30 2F 33 35 2E  65 78 65 07 63 6F 6D 6D  .130/35. exe.comm
00000075  61 6E 64 04 02 01 00 01  00 00 00 06 66 69 6C 65  and..... ....file
00000085  69 64 04 02 01 00 00 00  00 00                    id...... ..
```

## Packet 135

### Description

Infos about the game a buddy is playing

### Content

* `(nextSegmentLength 00 87 00 numberOfAtts)`
* `attribute_length 'sid' 04 03 number_of_sids 00 sid(16 bytes)`
* `attribute_length 'gameid' 04 02 number_of_games 00 gameid`
* `attribute_length 'gip' 04 02 number_of_ips 00 ip`
* `attribute_length 'gport' 04 02 number_of_ports 00 port(how coded?)`

### Example

```
                                   2A 00 87 00 04 03 73 69           *.....si
00000112  64 04 03 00 00 06 67 61  6D 65 69 64 04 02 00 00  d.....ga meid....
00000122  03 67 69 70 04 02 00 00  05 67 70 6F 72 74 04 02  .gip.... .gport..
00000132  00 00                                             ..
```

```
0000002C  46 00 87 00 04 03 73 69  64 04 03 01 00 25 07 CD  F.....si d....%..
0000003C  17 75 5B 93 58 06 7F 71  80 E2 56 BC 3C 06 67 61  .u[.X..q ..V.<.ga
0000004C  6D 65 69 64 04 02 01 00  40 11 00 00 03 67 69 70  meid.... @....gip
0000005C  04 02 01 00 00 00 00 00  05 67 70 6F 72 74 04 02  ........ .gport..
0000006C  01 00 00 00 00 00                                 ......
```

```
                      7E 00 87 00  04 03 73 69 64 04 03 03  .<.a~... ..sid...
0000036C  00 26 B4 4C 0E 0A 5F 2C  E1 2C BD 77 73 A0 25 69  .&.L.._, .,.ws.%i
0000037C  13 ED 0A 6B 01 3C 7F 1F  6E 41 EE DC 40 50 C1 89  ...k.<.. nA..@P..
0000038C  BA 13 96 FA 86 18 22 3A  1C 90 14 BE 6A A0 A7 8B  ......": ....j...
0000039C  FD 06 67 61 6D 65 69 64  04 02 03 00 F9 10 00 00  ..gameid ........
000003AC  56 10 00 00 78 10 00 00  03 67 69 70 04 02 03 00  V...x... .gip....
000003BC  00 00 00 00 E6 D4 EF D5  00 00 00 00 05 67 70 6F  ........ .....gpo
000003CC  72 74 04 02 03 00 00 00  00 00 CF 3C 00 00 00 00  rt...... ...<....
000003DC  00 00
```

* battlefield vietnam:

```
0000020D  24 00 8E 00 02 03 73 69  64 03 BC C3 6D 2E 96 74  $.....si d...m..t
0000021D  75 84 3B 8F CC 78 47 F6  A5 C8 04 74 79 70 65 02  u.;..xG. ...type.
0000022D  00 00 00 00                                       ....
00000231  46 00 87 00 04 03 73 69  64 04 03 01 00 BC C3 6D  F.....si d......m
00000241  2E 96 74 75 84 3B 8F CC  78 47 F6 A5 C8 06 67 61  ..tu.;.. xG....ga
00000251  6D 65 69 64 04 02 01 00  56 10 00 00 03 67 69 70  meid.... V....gip
00000261  04 02 01 00 00 00 00 00  05 67 70 6F 72 74 04 02  ........ .gport..
00000271  01 00 00 00 00 00                                 ......
000002BD  46 00 87 00 04 03 73 69  64 04 03 01 00 BC C3 6D  F.....si d......m
000002CD  2E 96 74 75 84 3B 8F CC  78 47 F6 A5 C8 06 67 61  ..tu.;.. xG....ga
000002DD  6D 65 69 64 04 02 01 00  56 10 00 00 03 67 69 70  meid.... V....gip
000002ED  04 02 01 00 22 08 26 CF  05 67 70 6F 72 74 04 02  ....".&. .gport..
000002FD  01 00 FC 6C 00 00                                 ...l..
00000303  46 00 87 00 04 03 73 69  64 04 03 01 00 BC C3 6D  F.....si d......m
00000313  2E 96 74 75 84 3B 8F CC  78 47 F6 A5 C8 06 67 61  ..tu.;.. xG....ga
00000323  6D 65 69 64 04 02 01 00  56 10 00 00 03 67 69 70  meid.... V....gip
00000333  04 02 01 00 6D E7 70 D4  05 67 70 6F 72 74 04 02  ....m.p. .gport..
00000343  01 00 CF 3C 00 00                                 ...<..
```

## Packet 137

### Description

Result of add buddy

### Content

* `packet_length 00 packet_type(137) 00 numberOfAtts`
* `attribute_length 'name' number_of_names name_length 00 name`

<br />

* `(attribute_length 'reason' 02 00 00 00 00) //error`

or

* `(attribute_length 'result' 02 00 00 00 00) //success`

### Example

```
00000000  1D 00 89 00 02 04 6E 61  6D 65 01 04 00 73 73 64  ......na me...ssd
00000010  66 06 72 65 73 75 6C 74  02 00 00 00 00           f.result .....
```

```
00000000  23 00 89 00 02 04 6E 61  6D 65 01 0A 00 6B 65 6E  #.....na me...ken
00000010  73 68 69 6E 33 33 33 06  72 65 73 75 6C 74 02 00  shin333. result..
00000020  00 00 00                                          ...
```

## Packet 138

### Description

Invitiation to be added to the contact list

### Content

* `packet_length 00 packet_type(138) 00 numberOfAtts`
* `attribute_length 'name' 04 01 01 00 name_length 00 name`
* `attribute_length 'nick' 04 01 01 00 nick_length 00`
* `attribute_length 'msg' 04 01 01 00 message_length 00 message`

### Example

```
                               48  00 8A 00 03 04 6E 61 6D         H .....nam
00000112  65 04 01 01 00 08 00 62  6C 69 74 7A 31 31 31 04  e......b litz111.
00000122  6E 69 63 6B 04 01 01 00  00 00 03 6D 73 67 04 01  nick.... ...msg..
00000132  01 00 1B 00 41 64 64 20  6D 65 20 74 6F 20 79 6F  ....Add  me to yo
00000142  75 72 20 46 72 69 65 6E  64 73 20 6C 69 73 74     ur Frien ds list
```

## Packet 139

### Description

Ack for remove buddy

### Content

* `packet_length type(139) number_of_atts`

### Example

```
00000000  11 00 8B 00 01 06 75 73  65 72 69 64 02 AB 3C 02  ......us erid..<.
00000010  00 
```

## Packet 142

### Description

Online(away) status information

### Content

* `(nextSegmentLength 00 8e 00 numberOfAtts)`
* `attribute_length 'sid' 03 sid(16bytes)`
* `attribute_length 'type' 02 01 00 00 00`
* `attribute_length 'status' 05 01`
* `attribute_length 't' statusLength_length statusLength 00 status`

### Example

```
00000132        4A 00 8E 00 03 03  73 69 64 03 8A AB D5 34    J..... sid....4
00000142  62 16 27 D7 0A EE 01 DE  DD 5B FD 95 04 74 79 70  b.'..... .[...typ
00000152  65 02 01 00 00 00 06 73  74 61 74 75 73 05 01 01  e......s tatus...
00000162  74 01 18 00 28 41 46 4B  29 20 41 77 61 79 20 46  t...(AFK ) Away F
00000172  72 6F 6D 20 4B 65 79 62  6F 61 72 64              rom Keyb oard
```

This one comes if a user is back online

```
0000023D  38 00 8E 00 03 03 73 69  64 03 BE D8 A8 53 B1 36  8.....si d....S.6
0000024D  3B E0 C0 E2 3E 6A C6 04  FA 29 04 74 79 70 65 02  ;...>j.. .).type.
0000025D  01 00 00 00 06 73 74 61  74 75 73 05 01 01 74 01  .....sta tus...t.
0000026D  06 00 4F 6E 6C 69 6E 65                           ..Online 
```

This one comes if a user is back online

```
0000004A  24 00 8E 00 02 03 73 69  64 03 0F A6 AE EB D3 B1  $.....si d.......
0000005A  32 99 C3 8A 07 02 92 A8  09 5D 04 74 79 70 65 02  2....... .].type.
0000006A  00 00 00 00                                       ....
```

## Packet 145

### Description

This error tells us that somebody logged in as us from somewhere else

### Content

* `packet_length 00 packet_type(145) 00 numberOfAtts`
* `attribute_length 'reason' 02 01 00 00 00`

### Example

```
0000022D  11 00 91 00 01 06 72 65  61 73 6F 6E 02 01 00 00  ......re ason....
0000023D  00 
```

```
00000389  2B 00 91 00 03 06 72 65  61 73 6F 6E 02 01 00 00  +.....re ason....
00000399  00 07 6D 69 6E 72 65 63  74 02 00 00 00 00 07 6D  ..minrec t......m
000003A9  61 78 72 65 63 74 02 00  00 00 00                 axrect.. ...
```

## Packet 148

### Description

Some game information? No username or so...

### Content

* `(nextSegmentLength type(131) 00 numberOfAtts)`
* `attribute_length 'max'`
* `attribute_length 'gameid'`
* `attribute_length 'gip'`
* `attribute_length 'gport'`

### Example

```
00000098                                                37  7
000000A8  00 94 00 04 03 6D 61 78  02 1E 00 00 00 06 67 61  .....max ......ga
000000B8  6D 65 69 64 04 02 01 00  09 10 00 00 03 67 69 70  meid.... .....gip
000000C8  04 02 01 00 4E DB 13 43  05 67 70 6F 72 74 04 02  ....N..C .gport..
000000D8  01 00 E7 38 00 00                                 ...8..
```

Packet is 654 (28e) bytes long

```
00000138  XX 8E 02 83 00 03 07 66  72 69 65 6E 64 73 04 01  0......f riends..
00000148  1E 00 0A 00 6A 65 73 75  73 66 72 65 61 6B 05 00  ....jesu sfreak..
00000158  67 6C 6F 62 65 09 00 6D  61 74 63 68 62 6F 6F 6B  globe..m atchbook
00000168  06 00 6B 65 61 6D 6F 73  08 00 64 33 66 31 61 6E  ..keamos ..d3f1an
00000178  63 33 09 00 70 6C 6F 6B  6B 72 31 32 33 0C 00 65  c3..plok kr123..e
00000188  65 74 75 72 77 65 65 64  69 65 73 09 00 62 69 67  eturweed ies..big
00000198  73 63 68 69 6D 73 08 00  61 73 72 61 6E 69 65 6C  schims.. asraniel
000001A8  06 00 6B 6F 6F 6C 69 6F  06 00 63 68 61 6F 73 6B  ..koolio ..chaosk
000001B8  08 00 70 6F 69 6E 74 33  31 34 05 00 6F 72 69 61  ..point3 14..oria
000001C8  68 0E 00 61 6C 62 69 6E  6F 63 68 69 6E 61 6D 61  h..albin ochinama
000001D8  6E 0B 00 75 6E 68 6F 6C  79 6D 61 6B 65 72 09 00  n..unhol ymaker..
000001E8  64 61 64 64 79 6E 6F 6F  6F 0C 00 64 6F 72 6B 79  daddynoo o..dorky
000001F8  74 69 66 66 61 6E 79 09  00 6C 61 7A 65 72 6D 61  tiffany. .lazerma
00000208  6E 65 06 00 62 65 65 66  67 74 0C 00 69 61 6D 74  ne..beef gt..iamt
00000218  68 65 6B 69 6C 6C 65 72  08 00 62 6C 69 74 7A 31  hekiller ..blitz1
00000228  31 31 07 00 6D 61 64 67  75 6E 61 0A 00 66 6C 69  11..madg una..fli
00000238  70 73 74 79 6C 65 65 07  00 65 63 6F 6D 69 6B 65  pstylee. .ecomike
00000248  0F 00 66 6F 73 68 65 65  7A 79 6D 79 6E 65 65 7A  ..foshee zymyneez
00000258  79 09 00 69 61 6D 6D 6F  75 73 65 79 07 00 61 70  y..iammo usey..ap
00000268  68 65 74 74 6F 0C 00 69  6C 6C 73 6B 69 6C 6C 73  hetto..i llskills
00000278  37 31 39 07 00 64 73 79  70 68 65 72 0D 00 70 68  719..dsy pher..ph
00000288  6F 65 6E 69 78 69 6E 71  75 69 73 04 6E 69 63 6B  oenixinq uis.nick
00000298  04 01 1E 00 0A 00 4A 65  73 75 73 46 72 65 61 6B  ......Je susFreak
000002A8  00 00 09 00 4D 61 74 63  68 62 6F 6F 6B 06 00 4B  ....Matc hbook..K
000002B8  65 61 6D 6F 73 08 00 44  65 66 69 61 6E 63 65 00  eamos..D efiance.
000002C8  00 04 00 6C 6F 4F 6C 09  00 42 69 67 53 63 68 69  ...loOl. .BigSchi
000002D8  6D 73 00 00 06 00 4B 6F  6F 6C 69 6F 0C 00 43 68  ms....Ko olio..Ch
000002E8  61 6F 73 20 4B 69 6C 6C  65 72 00 00 00 00 00 00  aos Kill er......
000002F8  06 00 55 6E 68 6F 6C 79  00 00 00 00 09 00 4C 61  ..Unholy ......La
00000308  7A 65 72 4D 61 6E 65 00  00 00 00 00 00 00 00 00  zerMane. ........
00000318  00 05 00 44 72 55 6E 4B  09 00 66 61 20 73 68 65  ...DrUnK ..fa she
00000328  65 7A 79 05 00 6D 6F 55  73 65 00 00 00 00 0A 00  ezy..moU se......
00000338  44 61 72 6B 53 79 70 68  65 72 00 00 06 75 73 65  DarkSyph er...use
00000348  72 69 64 04 02 1E 00 FC  31 01 00 79 32 01 00 D6  rid..... 1..y2...
00000358  33 01 00 AB 3C 02 00 A2  93 02 00 E3 3C 04 00 42  3...<... ....<..B
00000368  F9 04 00 7C AF 05 00 82  9C 09 00 05 24 0A 00 09  ...|.... ....$...
00000378  30 0A 00 41 A1 0A 00 0C  A4 0A 00 EC AE 0A 00 DD  0..A.... ........
00000388  CE 0A 00 C6 A1 0B 00 13  FA 0B 00 53 06 0C 00 CA  ........ ...S....
00000398  1C 0C 00 62 67 0C 00 09  01 0D 00 2F 8F 0D 00 7C  ...bg... .../...|
000003A8  31 0E 00 F3 A5 0E 00 E9  FA 10 00 C3 2C 11 00 D7  1....... ....,...
000003B8  7D 11 00 32 8D 11 00 8C  B2 12 00 6B 16 15 00 XX  }..2.... ...k....
```

## Packet 154

### Description

Away Message of a buddy

### Content

* `packet_length 00 packet_type(154) 00 numberOfAtts`
* `attribute_length 'sid' 04 03 01 00 01 sid(16 bytes)`
* `msg 04 01 01 00 msg_length 00 msg`

### Example

```
                3F 00 9A 00 02 03  73 69 64 04 03 01 00 01  rd?..... sid.....
0000046E  AF FC 8B 36 53 8F 0D 65  BF 8F 80 71 97 00 1B 03  ...6S..e ...q....
0000047E  6D 73 67 04 01 01 00 18  00 28 41 46 4B 29 20 41  msg..... .(AFK) A
0000048E  77 61 79 20 46 72 6F 6D  20 4B 65 79 62 6F 61 72  way From  Keyboar
0000049E  64                                                d
```

## Packet 141

### Description

Contact List

### Content

This is a LONG packet, which means, the length at the beginning is not the total length of the packet.

* `nextSegmentLength 00 8d 00 numberOfAtts`
* `attribute_length 'prefs' 05 00`
* `(nextSegmentLength 00 83 00 numberOfAtts)`
* `attribute_length 'friends' 04 01 02 00 userLength 00 userName userLength 00 userName`
* `attribute_length 'nick' 04 01 02 00 nickLength 00 nick nickLength 00 nick`
* `attribute_length 'userid' 04 02 02 00 userID userID`
* `(nextSegmentLength 00 84 00 numberOfAtts)`
* `attribute_length 'userid' 04 02 01 00 ab 3c 02 00`
* `attribute_length 'sid' 04 03 01 00 sid(16bytes)`
* `(nextSegmentLength 00 87 00 numberOfAtts)`
* `attribute_length 'sid' 04 03 00 00`
* `attribute_length 'gameid' 04 02 00 00`
* `attribute_length 'gip' 04 02 00 00`
* `attribute_length 'gport' 04 02 00 00`
* `(nextSegmentLength 00 8e 00 numberOfAtts)`
* `attribute_length 'sid' sid(17bytes)`
* `attribute_length 'type' 02 01 00 00 00`
* `attribute_length 'status' 05 01 01 74 statusLength_length statusLength 00 status`
* `(nextSegmentLength 00 93 00 numberOfAtts)`
* `attribute_length 'sid' 04 03 01 00 sid(17bytes)`
* `attribute_length 'vid' 04 02 01 00 20 00 00 00`
* `attribute_length 'vip' 04 02 01 00 5e 6a 13 43`
* `attribute_length 'vport' 04 02 01 00 3f 22 00 00`
* `(nextSegmentLength 00 87 00 numberOfAtts)`
* `......`
* `(nextSegmentLength 00 94 00 numberOfAtts)`
* `......`
* `(nextSegmentLength 00 90 01 numberOfAtts) //aha!`

### Example

```
00000082  0D 00 8D 00 01 05 70 72  65 66 73 05 00 4F 00 83  ......pr efs..O..
00000092  00 03 07 66 72 69 65 6E  64 73 04 01 02 00 06 00  ...frien ds......
000000A2  6B 65 61 6D 6F 73 0E 00  74 65 73 74 66 72 69 65  keamos.. testfrie
000000B2  6E 64 32 38 38 36 04 6E  69 63 6B 04 01 02 00 06  nd2886.n ick.....
000000C2  00 4B 65 61 6D 6F 73 00  00 06 75 73 65 72 69 64  .Keamos. ..userid
000000D2  04 02 02 00 AB 3C 02 00  AD 3A 06 00 2C 00 84 00  .....<.. .:..,...
000000E2  02 06 75 73 65 72 69 64  04 02 01 00 AB 3C 02 00  ..userid .....<..
000000F2  03 73 69 64 04 03 01 00  8A AB D5 34 62 16 27 D7  .sid.... ...4b.'.
00000102  0A EE 01 DE DD 5B FD 95  2A 00 87 00 04 03 73 69  .....[.. *.....si
00000112  64 04 03 00 00 06 67 61  6D 65 69 64 04 02 00 00  d.....ga meid....
00000122  03 67 69 70 04 02 00 00  05 67 70 6F 72 74 04 02  .gip.... .gport..
00000132  00 00 4A 00 8E 00 03 03  73 69 64 03 8A AB D5 34  ..J..... sid....4
00000142  62 16 27 D7 0A EE 01 DE  DD 5B FD 95 04 74 79 70  b.'..... .[...typ
00000152  65 02 01 00 00 00 06 73  74 61 74 75 73 05 01 01  e......s tatus...
00000162  74 01 18 00 28 41 46 4B  29 20 41 77 61 79 20 46  t...(AFK ) Away F
00000172  72 6F 6D 20 4B 65 79 62  6F 61 72 64 43 00 93 00  rom Keyb oardC...
00000182  04 03 73 69 64 04 03 01  00 8A AB D5 34 62 16 27  ..sid... ....4b.'
00000192  D7 0A EE 01 DE DD 5B FD  95 03 76 69 64 04 02 01  ......[. ..vid...
000001A2  00 20 00 00 00 03 76 69  70 04 02 01 00 5E 6A 13  . ....vi p....^j.
000001B2  43 05 76 70 6F 72 74 04  02 01 00 3F 22 00 00 46  C.vport. ...?"..F
000001C2  00 87 00 04 03 73 69 64  04 03 01 00 60 20 5B 52  .....sid ....` [R
000001D2  C2 F0 81 51 A3 CF DC 85  2B 8C 20 B8 06 67 61 6D  ...Q.... +. ..gam
000001E2  65 69 64 04 02 01 00 D7  10 00 00 03 67 69 70 04  eid..... ....gip.
000001F2  02 01 00 1E 23 ED 40 05  67 70 6F 72 74 04 02 01  ....#.@. gport...
00000202  00 20 71 00 00 2B 00 94  00 04 03 6D 61 78 02 1E  . q..+.. ...max..
00000212  00 00 00 06 67 61 6D 65  69 64 04 02 00 00 03 67  ....game id.....g
00000222  69 70 04 02 00 00 05 67  70 6F 72 74 04 02 00 00  ip.....g port....
00000232  1F 00 90 01 01 03 64 69  64 06 D1 3D 67 48 7B 21  ......di d..=gH{!
00000242  0C 8D 56 CC 72 90 7A 4A  AB F1 9D 2E 2C 4B 1B     ..V.r.zJ ....,K.
```

## Packet 144

### Description

Another Contact List

### Content

### Example

```
00000082  1F 00 90 01 01 03 64 69  64 06 D1 25 35 B0 95 9C  ......di d..%5...
00000092  35 41 D5 4E C0 3C 69 5D  D4 BB C8 B0 B7 9F 4F 0D  5A.N.<i] ......O.
000000A2  00 8D 00 01 05 70 72 65  66 73 05 00 4F 00 83 00  .....pre fs..O...
000000B2  03 07 66 72 69 65 6E 64  73 04 01 02 00 06 00 6B  ..friend s......k
000000C2  65 61 6D 6F 73 0E 00 74  65 73 74 66 72 69 65 6E  eamos..t estfrien
000000D2  64 32 38 38 36 04 6E 69  63 6B 04 01 02 00 06 00  d2886.ni ck......
000000E2  4B 65 61 6D 6F 73 00 00  06 75 73 65 72 69 64 04  Keamos.. .userid.
000000F2  02 02 00 AB 3C 02 00 AD  3A 06 00 2C 00 84 00 02  ....<... :..,....
00000102  06 75 73 65 72 69 64 04  02 01 00 AB 3C 02 00 03  .userid. ....<...
00000112  73 69 64 04 03 01 00 8C  73 95 43 8D 08 3C 53 49  sid..... s.C..<SI
00000122  30 A9 76 B8 56 38 2F 2A  00 87 00 04 03 73 69 64  0.v.V8/* .....sid
00000132  04 03 00 00 06 67 61 6D  65 69 64 04 02 00 00 03  .....gam eid.....
00000142  67 69 70 04 02 00 00 05  67 70 6F 72 74 04 02 00  gip..... gport...
00000152  00 46 00 87 00 04 03 73  69 64 04 03 01 00 6D A0  .F.....s id....m.
00000162  D1 11 0F 5B E1 E4 28 C0  FD 6E 79 F5 BF 79 06 67  ...[..(. .ny..y.g
00000172  61 6D 65 69 64 04 02 01  00 78 10 00 00 03 67 69  ameid... .x....gi
00000182  70 04 02 01 00 51 E1 81  0C 05 67 70 6F 72 74 04  p....Q.. ..gport.
00000192  02 01 00 8C 0E 00 00 2B  00 94 00 04 03 6D 61 78  .......+ .....max
000001A2  02 1E 00 00 00 06 67 61  6D 65 69 64 04 02 00 00  ......ga meid....
000001B2  03 67 69 70 04 02 00 00  05 67 70 6F 72 74 04 02  .gip.... .gport..
000001C2  00 00                                             ..
```

```
00000078  1F 00 90 01 01 03 64 69  64 06 D1 20 6B 88 D7 69  ......di d.. k..i
00000088  90 15 7F 50 94 7C F3 2E  C9 E0 66 15 6A 85 96 2B  ...P.|.. ..f.j..+
00000098  00 94 00 04 03 6D 61 78  02 1E 00 00 00 06 67 61  .....max ......ga
000000A8  6D 65 69 64 04 02 00 00  03 67 69 70 04 02 00 00  meid.... .gip....
000000B8  05 67 70 6F 72 74 04 02  00 00 0D 00 8D 00 01 05  .gport.. ........
000000C8  70 72 65 66 73 05 00 3B  00 83 00 03 07 66 72 69  prefs..; .....fri
000000D8  65 6E 64 73 04 01 01 00  0E 00 74 65 73 74 66 72  ends.... ..testfr
000000E8  69 65 6E 64 32 38 38 36  04 6E 69 63 6B 04 01 01  iend2886 .nick...
000000F8  00 00 00 06 75 73 65 72  69 64 04 02 01 00 AD 3A  ....user id.....:
00000108  06 00 2A 00 87 00 04 03  73 69 64 04 03 00 00 06  ..*..... sid.....
00000118  67 61 6D 65 69 64 04 02  00 00 03 67 69 70 04 02  gameid.. ...gip..
00000128  00 00 05 67 70 6F 72 74  04 02 00 00 96 01 87 00  ...gport ........
00000138  04 03 73 69 64 04 03 0D  00 2A 44 E6 1D 08 5C 99  ..sid... .*D...\.
00000148  50 5B 3B 99 8E 73 0D D1  08 70 85 41 28 6E 68 0C  P[;..s.. .p.A(nh.
00000158  38 07 F3 50 D6 5C 9F 4B  86 D4 BE CC 21 D1 6E 6B  8..P.\.K ....!.nk
00000168  E3 EB C5 8E BB 00 D0 43  53 EB F7 C5 05 6A 78 95  .......C S....jx.
00000178  3C D6 82 3C 57 9C 29 F2  EB 5C CD 2B BD A0 D5 6F  <..<W.). .\.+...o
00000188  D3 B3 35 E0 0A 30 26 F0  AD F6 BB F8 97 42 3D 53  ..5..0&. .....B=S
00000198  5F CE 92 70 EE 72 4F 96  F1 45 48 08 4F 2A 54 67  _..p.rO. .EH.O*Tg
000001A8  D8 25 BD AD 3F E1 34 40  3C 33 27 CB A7 86 0C 26  .%..?.4@ <3'....&
000001B8  54 6D C3 D1 BF 0E 18 C6  ED 23 CB 52 CC 8D E1 94  Tm...... .#.R....
000001C8  B3 22 5E 3E 08 79 44 C1  A2 DC A8 A1 F1 4C CC 97  ."^>.yD. .....L..
000001D8  B8 5E 3A 5F 38 28 DD CB  F4 77 50 B4 06 4A 70 4C  .^:_8(.. .wP..JpL
000001E8  20 1A A2 EC 3C 9A D2 76  63 EE 9B 3C 05 C5 8F 57  ...<..v c..<...W
000001F8  1E 51 F9 AA E6 7C 00 96  03 C4 1A 44 6C 6B D7 D3  .Q...|.. ...Dlk..
00000208  AC 2A C4 F7 89 38 01 B4  BF 06 67 61 6D 65 69 64  .*...8.. ..gameid
00000218  04 02 0D 00 AE 10 00 00  77 10 00 00 E3 10 00 00  ........ w.......
00000228  86 10 00 00 09 10 00 00  44 10 00 00 0F 10 00 00  ........ D.......
00000238  45 10 00 00 18 10 00 00  05 00 00 00 05 00 00 00  E....... ........
00000248  BD 10 00 00 05 00 00 00  03 67 69 70 04 02 0D 00  ........ .gip....
00000258  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ........ ........
00000268  09 E7 70 D4 14 EC F2 C3  00 00 00 00 18 34 8E 53  ..p..... .....4.S
00000278  F3 BB 0F 51 10 E9 1C C1  03 92 62 53 30 6A F8 D5  ...Q.... ..bS0j..
00000288  51 AB 02 51 05 67 70 6F  72 74 04 02 0D 00 00 00  Q..Q.gpo rt......
00000298  00 00 00 00 00 00 00 00  00 00 00 00 00 00 E7 38  ........ .......8
000002A8  00 00 87 69 00 00 00 00  00 00 87 69 00 00 20 71  ...i.... ...i.. q
000002B8  00 00 8A 69 00 00 87 69  00 00 A0 0F 00 00 A5 69  ...i...i .......i
000002C8  00 00                                             ..
```

# Chat

## Receive IM (133)

### Description

We got a chat message. We have to send a Ack for this IM

### Content


1.  sid between the sender and me
2.  peermesage(?)
3.  msgtype
4.  imindex
5.  im, the message

* `length 00 type(133) 00 numberOfAtts //all after and with peermsg is 1 att`
* `attribute_length 'sid' 03 SID //sid_length = 16`
* `attribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 00 00 00 00`
* `attribute_length 'imindex' 02 numberOfMessage(xx 00 00 00)`
* `attribute_length 'im' messageLength_length messageLength 00 messageText`

### Example

```
000000D9  58 00 85 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  X.....si d.î.<.Å.
000000E9  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Qùªæ|. ...peerm
000000F9  73 67 05 03 07 6D 73 67  74 79 70 65 02 00 00 00  sg...msg type....
00000109  00 07 69 6D 69 6E 64 65  78 02 02 00 00 00 02 69  ..iminde x......i
00000119  6D 01 14 00 6F 6B 20 6F  6B 20 28 69 6D 20 66 72  m...ok o k (im fr
00000129  65 6E 63 68 29 20 6E 70                           ench) np
```

## Ack IM (02)

### Description

We have to ack the IM we got, so that the chatpartner know we got it.

### Content

1. sid between the sender and me
2. peermesage(?)
3. msgtype
4. imindex

* `3e 00 type(02) 00 numberOfAtts //all after and with peermsg is 1 att`
* `attribute_length 'sid' 03 SID`
* `ttribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 01 00 00 00`
* `attribute_length 'imindex' 02 numberOfMessage(xx 00 00 00)`

### Example

```
0000011C  3E 00 02 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  >.....si d.î.<.Å.
0000012C  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Qùªæ|. ...peerm
0000013C  73 67 05 02 07 6D 73 67  74 79 70 65 02 01 00 00  sg...msg type....
0000014C  00 07 69 6D 69 6E 64 65  78 02 02 00 00 00        ..iminde x.....
```

```
0000011C  3E 00 02 00 02 03 73 69  64 03 01 D5 DB 88 D3 9C  >.....sid.......
0000012C  39 1F F4 74 BF 8D 3B E3  56 18 07 70 65 65 72 6D  9..t..;.V..peerm
0000013C  73 67 05 02 07 6D 73 67  74 79 70 65 02 01 00 00  sg...msgtype....
0000014C  00 07 69 6D 69 6E 64 65  78 02 0A 00 00 00        ..imindex.....
```

## Send IM (02)

### Description

We have Send a IM. We will receive a Ack for our IM

### Content

1. sid between the destination and me
2. peermesage(?)
3. msgtype
4. imindex
5. im

* `length 00 type(02) 00 numberOfAtts //all after and with peermsg is 1 att`
* `attribute_length 'sid' 03 SID`
* `attribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 00 00 00 00`
* `attribute_length 'imindex' 02 numberOfMessage(xx 00 00 00)`
* `attribute_length 'im' messageLength_length messageLength 00 messageText`

### Example

```
00000198  57 00 02 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  W.....si d.î.<.Å.
000001A8  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Qùªæ|. ...peerm
000001B8  73 67 05 03 07 6D 73 67  74 79 70 65 02 00 00 00  sg...msg type....
000001C8  00 07 69 6D 69 6E 64 65  78 02 04 00 00 00 02 69  ..iminde x......i
000001D8  6D 01 13 00 69 20 61 6C  73 6F 20 73 70 65 61 6B  m...i al so speak
000001E8  20 66 72 65 6E 63 68                              french
```

```
0000009B  48 00 02 00 02 03 73 69  64 03 01 73 00 12 82 82  H.....si d..s....
000000AB  1D AF E8 D5 25 E7 FE 99  C5 8E 07 70 65 65 72 6D  ....%... ...peerm
000000BB  73 67 05 03 07 6D 73 67  74 79 70 65 02 00 00 00  sg...msg type....
000000CB  00 07 69 6D 69 6E 64 65  78 02 02 00 00 00 02 69  ..iminde x......i
000000DB  6D 01 04 00 61 73 64 66                           m...asdf
```

## Ack for our IM (133)

### Description

We got a ack for the IM we sent

### Content

1. sid between the destination and me
2. peermesage(?)
3. msgtype
4. imindex

* `3e 00 type(133) 00 numberOfAtts //all after and with peermsg is 1 att`
* `attribute_length 'sid' 03 SID`
* `attribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 01 00 00 00`
* `attribute_length 'imindex' 02 numberOfMessage(xx 00 00 00)`

### Example

```
00000177  3E 00 85 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  >.....si d.î.<.Å.
00000187  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Qùªæ|. ...peerm
00000197  73 67 05 02 07 6D 73 67  74 79 70 65 02 01 00 00  sg...msg type....
000001A7  00 07 69 6D 69 6E 64 65  78 02 04 00 00 00        ..iminde x.....
```

## Chat Authentication (133)

### Description

Our chat partner want some infos about us, and gives us a salt.. dont know for what. We have to respond

### Content

1. sid of the sender
2. peermesage(?)
3. msgtype
4. ip (public?)
5. port(public?)
6. localip
7. localport
8. status
9. salt

* `length 00 type(133) 00 numberOfAtts //all after and with peermsg is 1 att `
* `attribute_length 'sid' 03 SID`
* `attribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 02 00 00 00`
* `attribute_length 'ip' 02 ipInHex`
* `attribute_length 'port' 02 25 04 00 00`
* `attribute_length 'localip' 02 ipInHex`
* `attribute_length 'localport' 02 25 04 00 00 06`
* `attribute_length 'status' 02 01 00 00 00`
* `attribute_length 'salt' saltLength_length saltLength 00 salt(40bytes)`

### Example

```
0000003E  9B 00 85 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  ......si d...<...
0000004E  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Q...|. ...peerm
0000005E  73 67 05 07 07 6D 73 67  74 79 70 65 02 02 00 00  sg...msg type....
0000006E  00 02 69 70 02 18 74 70  53 04 70 6F 72 74 02 25  ..ip..tp S.port.%
0000007E  04 00 00 07 6C 6F 63 61  6C 69 70 02 18 74 70 53  ....loca lip..tpS
0000008E  09 6C 6F 63 61 6C 70 6F  72 74 02 25 04 00 00 06  .localpo rt.%....
0000009E  73 74 61 74 75 73 02 01  00 00 00 04 73 61 6C 74  status.. ....salt
000000AE  01 28 00 65 62 65 66 66  66 38 33 36 39 62 63 32  .(.ebeff f8369bc2
000000BE  65 37 39 30 38 35 62 38  36 34 35 34 32 36 66 64  e79085b8 645426fd
000000CE  36 34 33 30 30 62 66 66  37 34 32                 64300bff 742
```

## Chat Authentication response(133)

### Description

We send our chat partner some infos and his hash.

### Content

1. sid of the sender
2. peermesage(?)
3. msgtype
4. ip (public?)
5. port(public?)
6. localip
7. localport
8. status
9. salt

* `length 00 type(2) 00 numberOfAtts //all after and with peermsg is 1 att`
* `attribute_length 'sid' 03 SID //sid of the other`
* `attribute_length 'peermsg' 05 numberOfAtts`
* `attribute_length 'msgtype' 02 02 00 00 00`
* `attribute_length 'ip' 02 ipInHex`
* `attribute_length 'port' 02 25 04 00 00`
* `attribute_length 'localip' 02 ipInHex`
* `attribute_length 'localport' 02 25 04 00 00 06`
* `attribute_length 'status' 02 01 00 00 00`
* `attribute_length 'salt' saltLength_length saltLength 00 salt(40bytes)`

### Example

```
00000081  9B 00 02 00 02 03 73 69  64 03 EE 9B 3C 05 C5 8F  ......si d...<...
00000091  57 1E 51 F9 AA E6 7C 00  96 03 07 70 65 65 72 6D  W.Q...|. ...peerm
000000A1  73 67 05 07 07 6D 73 67  74 79 70 65 02 02 00 00  sg...msg type....
000000B1  00 02 69 70 02 B0 18 A7  3E 04 70 6F 72 74 02 F3  ..ip.... >.port..
000000C1  6A 00 00 07 6C 6F 63 61  6C 69 70 02 21 01 A8 C0  j...loca lip.!...
000000D1  09 6C 6F 63 61 6C 70 6F  72 74 02 96 06 00 00 06  .localpo rt......
000000E1  73 74 61 74 75 73 02 01  00 00 00 04 73 61 6C 74  status.. ....salt
000000F1  01 28 00 65 62 65 66 66  66 38 33 36 39 62 63 32  .(.ebeff f8369bc2
00000101  65 37 39 30 38 35 62 38  36 34 35 34 32 36 66 64  e79085b8 645426fd
00000111  36 34 33 30 30 62 66 66  37 34 32                 64300bff 742
```

## Packet 143

### Description

Ask for buddy add

### Content

* `packet_length 00 type(143) 00 numberOfAtts
* `attribute_length 'name' number_of_names name_length 00 name`
* `attribute_length 'fname' number_of_fnames fname_length 00 fname //first name`
* `attribute_length 'lname' number_of_lnames lname_length 00 lname //last name`

### Example

```
00000736  3B 00 8F 00 03 04 6E 61  6D 65 04 01 01 00 06 00  ;.....na me......
00000746  6B 65 61 6D 6F 73 05 66  6E 61 6D 65 04 01 01 00  keamos.f name....
00000756  07 00 4D 69 63 68 61 65  6C 05 6C 6E 61 6D 65 04  ..Michae l.lname.
00000766  01 01 00 06 00 43 61 6C  64 65 72                 .....Cal der
```
