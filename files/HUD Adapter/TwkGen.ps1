param (
    [int]$Width = (Read-Host -Prompt "Enter screen width"),
    [int]$Height = (Read-Host -Prompt "Enter screen height"),
    [switch]$NoPrompts
)

$ErrorActionPreference = "Stop"

[float]$AspectRatio = $Width / $Height
[float]$Offset = [Math]::Floor($AspectRatio * 240 - 320)
[float]$OffsetC = [Math]::Ceiling($AspectRatio * 240 - 320)
[float]$ArrowOffset = [Math]::Round($AspectRatio * 21.6 - 4.8)
[float]$ArrowOrient = [Math]::Round(-($AspectRatio * 0.135 + 0.02), 3)
$OutDir = "Output_{0}x{1}" -f $Width, $Height
$Comment = @'

<!--
  Changes:
    + Widescreen fixes ({0}:{1})
-->
'@ -f $Width, $Height
$CommentAnyScreen = @'

<!--
  Changes:
    + Widescreen fixes
-->
'@

function EditVal($content, $path, $value) {
    $container,$id,$field = $path -split(":")
    $l = $content |
    Select-String -Pattern "<$container id='$id'>" |
    Select-Object -ExpandProperty 'LineNumber'
    while ($content[$l] -NotMatch "</$container>") {
        if ($content[$l] -Match "<$field") {
            if ($content[$l] -Match ">(-?\d+)<") {
                # $content[$l] # before
                [float]$old = $matches[1]
                $content[$l] = $content[$l] -Replace "(?<=>)-?\d+(?=<)", $value
                # $content[$l] # after
                Write-Host "$l $container $id $field : $old > $value"
            }
            break
        }
        $l++
    }
}

function EditAttr($content, $path, $value) {
    $container,$id,$field,$attr = $path -split(":")
    $l = $content |
    Select-String -Pattern "<$container id='$id'>" |
    Select-Object -ExpandProperty 'LineNumber'
    while ($content[$l] -NotMatch "</$container>") {
        if ($content[$l] -Match "<$field") {
            if ($content[$l] -Match "$attr='(-?[0-9.]+)'") {
                # $content[$l] # before
                [float]$old = $matches[1]
                $content[$l] = $content[$l] -Replace "(?<=$attr=')-?[0-9.]+(?=')", $value
                # $content[$l] # after
                Write-Host "$l $container $id $field $attr : $old > $value"
            }
            break
        }
        $l++
    }
}

Set-Location $PSScriptRoot
Remove-Item -Recurse -Force $OutDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Write-Host "HudTwk"
$HudTwk = Get-Content "Original\HudTwk.xml"
# Scanner
EditAttr $HudTwk "XVectorResourceDetails:HUD.Scanner.Position:Value:x" (-241 - $Offset)
EditAttr $HudTwk "XVectorResourceDetails:HUD.Scanner.OffscreenPos:Value:x" (-400 - $Offset)
# Wind
EditAttr $HudTwk "XVectorResourceDetails:HUD.Wind.OnScreenPos:Value:x" (-268 - $Offset)
EditAttr $HudTwk "XVectorResourceDetails:HUD.Wind.OffScreenPos:Value:x" (-400 - $Offset)
EditAttr $HudTwk "XVectorResourceDetails:HUD.Wind.ArrowOffset:Value:x" $ArrowOffset
EditAttr $HudTwk "XVectorResourceDetails:HUD.Wind.ArrowOrien:Value:z" $ArrowOrient
# Powerbar
EditAttr $HudTwk "XVectorResourceDetails:HUD.Powerbar.Position:Value:x" (-269 - $Offset)
# Angle Meter
EditVal $HudTwk "XFloatResourceDetails:HUD.AngleMeter.OnScreenX:Value" (-215 - $Offset)
EditVal $HudTwk "XFloatResourceDetails:HUD.AngleMeter.OffScreenX:Value" (-215 - $Offset)
# Health & Teams Bar
EditAttr $HudTwk "XVectorResourceDetails:HUD.ActWormInfo.Pos:Value:x" (253 + $Offset)
EditAttr $HudTwk "XVectorResourceDetails:HUD.HPreview.Position:Value:x" (252 + $Offset)
# Clock
EditAttr $HudTwk "XVectorResourceDetails:HUD.Clock.Position:Value:x" (254 + $Offset)
# Counter (Challenges)
EditAttr $HudTwk "XVectorResourceDetails:HUD.Counter.Position:Value:x" (200 + $Offset)
$HudTwk[0] += $Comment
Set-Content -Path "$OutDir\HudTwk.xml" -Value $HudTwk
Write-Host ""

Write-Host "MenuTwkXInGame"
$MenuTwkXInGame = Get-Content "Original\MenuTwkXInGame.xml"
EditAttr $MenuTwkXInGame "WXFE_ListControlDesc:WXFE.HelpBlimpPC-0:Position:x" (-200 - $Offset)
EditAttr $MenuTwkXInGame "WXFE_ListControlDesc:WXFE.HelpDropPC-0:Position:x" (-200 - $Offset)
$MenuTwkXInGame[0] += $Comment
Set-Content -Path "$OutDir\MenuTwkXInGame.xml" -Value $MenuTwkXInGame
Write-Host ""

Write-Host "MenuTwkX"
$MenuTwkX = Get-Content "Original\MenuTwkX.xml"
# Story Intro Anim
EditAttr $MenuTwkX "WXFE_ImageViewDesc:WXFE.PlayMovieThenMenu-1:Scale:x" (320 + $OffsetC)
# Blue Divide Dark Background
EditAttr $MenuTwkX "WXFE_ImageViewDesc:WXFE.PreRound-6:Scale:x" 1000
$MenuTwkX[0] += $Comment
Set-Content -Path "$OutDir\MenuTwkX.xml" -Value $MenuTwkX
Write-Host ""

Write-Host "MenuTwkXPCCommon"
$MenuTwkXPCCommon = Get-Content "Original\MenuTwkXPCCommon.xml"
# Blue Divide Dark Background
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.Credits-9:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinAllMissions-5:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinAllTutorials-5:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinAward-8:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinChallenge-5:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinMatch-9:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinMission-5:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinRound-10:Scale:x" 1000
EditAttr $MenuTwkXPCCommon "WXFE_ImageViewDesc:WXFE.WinTutorial-5:Scale:x" 1000
$MenuTwkXPCCommon[0] += $CommentAnyScreen
Set-Content -Path "$OutDir\MenuTwkXPCCommon.xml" -Value $MenuTwkXPCCommon
Write-Host ""

Write-Host "MenuTwkXNetPC"
$MenuTwkXNetPC = Get-Content "Original\MenuTwkXNetPC.xml"
# Blue Divide Dark Background
EditAttr $MenuTwkXNetPC "WXFE_ImageViewDesc:WXNET.PreRound-6:Scale:x" 1000
EditAttr $MenuTwkXNetPC "WXFE_ImageViewDesc:WXNET.WinMatch-9:Scale:x" 1000
EditAttr $MenuTwkXNetPC "WXFE_ImageViewDesc:WXNET.WinRound-10:Scale:x" 1000
$MenuTwkXNetPC[0] += $CommentAnyScreen
Set-Content -Path "$OutDir\MenuTwkXNetPC.xml" -Value $MenuTwkXNetPC
Write-Host ""

Write-Host "PartTwk"
$PartTwk = Get-Content "Original\PartTwk.xml"
# Frontend Fireworks
EditAttr $PartTwk "ParticleEmitterContainer:WXPF_Whiteout-0:ParticleSize:x" 900
EditAttr $PartTwk "ParticleEmitterContainer:WXPF_WhiteoutDelayed-0:ParticleSize:x" 900
# Dim-Mak
EditAttr $PartTwk "ParticleEmitterContainer:WXP_Dim-Mak_Whiteout-0:ParticleSize:x" 900
# Fire Punch
EditAttr $PartTwk "ParticleEmitterContainer:WXP_FirePunchWhiteout-0:ParticleSize:x" 900
# Moon Physics
EditAttr $PartTwk "ParticleEmitterContainer:WXP_LOWGWhiteout-0:ParticleSize:x" 900
# Flood / Global Warming
EditAttr $PartTwk "ParticleEmitterContainer:WXP_StormDarken-0:ParticleSize:x" 900
# Telepad / Reposition Worm
EditAttr $PartTwk "ParticleEmitterContainer:WXP_TeleportWhiteout-0:ParticleSize:x" 900
# Alien Abduction
EditAttr $PartTwk "ParticleEmitterContainer:WXP_WarpgateScreenFlash-0:ParticleSize:x" 900
# Explosions
EditAttr $PartTwk "ParticleEmitterContainer:WXP_WhiteoutFlash-0:ParticleSize:x" 900
EditAttr $PartTwk "ParticleEmitterContainer:WXP_WhiteoutflashLarge-0:ParticleSize:x" 900
# Fall Damage
EditAttr $PartTwk "ParticleEmitterContainer:WXP_WormLandWhiteout-0:ParticleSize:x" 900
$PartTwk[0] += $CommentAnyScreen
Set-Content -Path "$OutDir\PartTwk.xml" -Value $PartTwk
Write-Host ""

Write-Host "Tutorial1"
$Tutorial1 = Get-Content "Original\Tutorial1.xml"
EditVal $Tutorial1 "EFMV_CreateCustomHudGraphicEventContainer:Hud.AimingPower-2:X" (-243 - $Offset)
EditVal $Tutorial1 "EFMV_CreateCustomHudGraphicEventContainer:Hud.Scanner-2:X" (-240 - $Offset)
$Tutorial1[0] += $Comment
Set-Content -Path "$OutDir\Tutorial1.xml" -Value $Tutorial1
Write-Host ""

Write-Host "Tutorial2"
$Tutorial2 = Get-Content "Original\Tutorial2.xml"
EditVal $Tutorial2 "EFMV_CreateCustomHudGraphicEventContainer:Hud.TurnTime-2:X" (255 + $Offset)
$Tutorial2[0] += $Comment
Set-Content -Path "$OutDir\Tutorial2.xml" -Value $Tutorial2
Write-Host ""

Write-Host "Tutorial3"
$Tutorial3 = Get-Content "Original\Tutorial3.xml"
EditVal $Tutorial3 "EFMV_CreateCustomHudGraphicEventContainer:HUD.Wind-2:X" (-250 - $Offset)
$Tutorial3[0] += $Comment
Set-Content -Path "$OutDir\Tutorial3.xml" -Value $Tutorial3
Write-Host ""

if (-Not $NoPrompts) {
    Write-Host "Done, press any key to exit..."
    $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") | Out-Null
}
else {
    Write-Host "Done..."
}
