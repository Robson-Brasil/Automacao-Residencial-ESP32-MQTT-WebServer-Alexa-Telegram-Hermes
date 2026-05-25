/**
 * @file WebServer.h - OTIMIZAÇÃO MÁXIMA (FLASH & RAM)
 */
#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#include <WebServer.h>
#include <Preferences.h>
#include "VariaveisGlobais.h"
#include "GPIOs.h"

void notifyTelegramStateChange(const String& source, int relayNum, bool relayState);
void saveRelayState(int relayNum, bool state);
void publishRelayStates();
void logRelayAction(const char* source, int relayNum, bool state);
extern bool mqttConnected;

// DASHBOARD MINIFICADO (Espaços e comentários removidos)
const char DEFAULT_NAMES[8][32] = {
    "Rei 1 - Luz Quarto",
    "Rei 2 - Luz Sala",
    "Rei 3 - Ventilador",
    "Rei 4 - Tomada 1",
    "Rei 5 - Tomada 2",
    "Rei 6 - Luz Cozinha",
    "Rei 7 - Ar Condicionado",
    "Rei 8 - Portao"
};

String getRelayName(int idx) {
    if (idx < 0 || idx > 7) return "";
    Preferences p;
    char key[16];
    snprintf(key, sizeof(key), "RelayName%d", idx + 1);
    p.begin("app", false);
    String name = p.getString(key, "");
    p.end();
    if (name == "") {
        return DEFAULT_NAMES[idx];
    }
    return name;
}

const char DASHBOARD_HTML[] PROGMEM = R"rawl(<!DOCTYPE html><html lang="pt-BR"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1.0"><title>ESP32 Automacao - Robson Brasil</title><style>@import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Outfit:wght@300;400;600;800&display=swap');:root{--bg:#060810;--surface:#0d1119;--surface2:#131b28;--border:#1a2235;--accent:#00ffc8;--accent2:#6c5ce7;--warn:#ff6b6b;--text:#e8ecf4;--text2:#8892a8;--glow:0 0 30px rgba(0,255,200,.3);--glass:rgba(13,17,25,.8)}*{box-sizing:border-box;margin:0;padding:0;outline:none !important;-webkit-tap-highlight-color:transparent}body{background:var(--bg);color:var(--text);font-family:'Outfit',sans-serif;min-height:100vh;overflow-x:hidden}body::before{content:'';position:fixed;inset:0;background:radial-gradient(ellipse 80% 50% at 50% -20%,rgba(0,255,200,.08),transparent),radial-gradient(ellipse 60% 40% at 100% 0%,rgba(108,92,231,.06),transparent),linear-gradient(rgba(0,255,200,.02) 1px,transparent 1px),linear-gradient(90deg,rgba(0,255,200,.02) 1px,transparent 1px);background-size:100% 100%,100% 100%,40px 40px,40px 40px;pointer-events:none;z-index:-1}header{padding:20px 32px;border-bottom:1px solid var(--border);display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;background:var(--glass);backdrop-filter:blur(20px);z-index:100}.logo{display:flex;align-items:center;gap:12px}.logo-dot{width:12px;height:12px;border-radius:50%;background:var(--accent);box-shadow:var(--glow);animation:pulse 2s ease-in-out infinite}@keyframes pulse{0%,100%{opacity:1;transform:scale(1)}50%{opacity:.6;transform:scale(1.2)}}.logo h1{font-size:1.2rem;font-weight:800;letter-spacing:.06em}.logo span{color:var(--accent)}.status-bar{display:flex;gap:16px;flex-wrap:wrap;justify-content:flex-end}.badge{font-family:'JetBrains Mono',monospace;font-size:.7rem;padding:6px 14px;border-radius:8px;border:1px solid;display:flex;align-items:center;gap:8px;transition:all .3s}.badge.online{border-color:var(--accent);color:var(--accent);background:rgba(0,255,200,.05)}.badge.offline{border-color:var(--warn);color:var(--warn);background:rgba(255,107,107,.05)}.badge-dot{width:6px;height:6px;border-radius:50%;background:currentColor;animation:blink 1.5s ease-in-out infinite}@keyframes blink{0%,100%{opacity:1}50%{opacity:.3}}main{max-width:1400px;margin:0 auto;padding:28px 24px;display:grid;gap:24px}.section-label{font-family:'JetBrains Mono',monospace;font-size:.65rem;letter-spacing:.2em;color:var(--text2);text-transform:uppercase;margin-bottom:16px;display:flex;align-items:center;gap:12px}.section-label::after{content:'';flex:1;height:1px;background:linear-gradient(90deg,var(--border),transparent)}.info-bar{background:linear-gradient(135deg,var(--surface),var(--surface2));border:1px solid var(--border);border-radius:16px;padding:20px 28px;display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:24px;position:relative;overflow:hidden}.info-bar::before{content:'';position:absolute;top:0;left:0;right:0;height:1px;background:linear-gradient(90deg,transparent,var(--accent),transparent);opacity:.5}.info-item{display:flex;flex-direction:column;gap:4px}.info-item label{font-family:'JetBrains Mono',monospace;font-size:.6rem;color:var(--text2);text-transform:uppercase;letter-spacing:.12em}.info-item span{font-family:'JetBrains Mono',monospace;font-size:.85rem;color:var(--accent)}.master-card{background:linear-gradient(135deg,var(--surface),var(--surface2));border:1px solid var(--border);border-radius:20px;padding:28px 32px;display:flex;align-items:center;justify-content:space-between;gap:24px;position:relative;transition:all .4s ease;cursor:pointer;overflow:hidden}.master-card:hover{transform:translateY(-2px)}.master-info h2{font-size:1.5rem;font-weight:800;margin-bottom:6px;background:linear-gradient(90deg,var(--text),var(--accent));-webkit-background-clip:text;-webkit-text-fill-color:transparent}.master-info p{font-size:.8rem;color:var(--text2);font-family:'JetBrains Mono',monospace}.master-toggle{display:flex;align-items:center;gap:16px}.toggle{position:relative;width:64px;height:32px;cursor:pointer}.toggle input{display:none}.toggle-track{position:absolute;inset:0;background:#1a2235;border-radius:16px;border:2px solid var(--border);transition:all .35s cubic-bezier(.4,0,.2,1)}.toggle input:checked~.toggle-track{background:rgba(0,255,200,.15);border-color:var(--accent);box-shadow:var(--glow),inset 0 0 20px rgba(0,255,200,.1)}.toggle-thumb{position:absolute;top:3px;left:3px;width:24px;height:24px;border-radius:50%;background:linear-gradient(135deg,#8892a8,#5a6278);transition:all .35s cubic-bezier(.4,0,.2,1);box-shadow:0 2px 8px rgba(0,0,0,.3)}.toggle input:checked~.toggle-track .toggle-thumb{transform:translateX(32px);background:linear-gradient(135deg,var(--accent),#00cc9e);box-shadow:0 0 20px rgba(0,255,200,.5)}.toggle-label{font-family:'JetBrains Mono',monospace;font-size:.75rem;color:var(--text2);min-width:90px;transition:color .3s}.toggle-label.active{color:var(--accent);text-shadow:0 0 10px rgba(0,255,200,.5)}.relay-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(240px,1fr));gap:18px}.relay-card{background:linear-gradient(145deg,var(--surface),var(--surface2));border:1px solid var(--border);border-radius:16px;padding:22px;cursor:pointer;transition:all .3s cubic-bezier(.4,0,.2,1);position:relative;overflow:hidden}.relay-card::before{content:'';position:absolute;top:0;left:0;right:0;height:3px;background:linear-gradient(90deg,var(--accent),var(--accent2));transform:scaleX(0);transition:transform .4s cubic-bezier(.4,0,.2,1)}.relay-card:hover{transform:translateY(-4px)}.relay-card.active::before{transform:scaleX(1)}.relay-header{display:flex;justify-content:center;align-items:center;margin-bottom:16px;position:relative;z-index:1}.relay-name{font-size:1rem;font-weight:600;text-align:center;cursor:pointer;width:100%}.relay-gpio{font-family:'JetBrains Mono',monospace;font-size:.7rem;color:var(--text2);margin-bottom:18px;position:relative;z-index:1}.relay-status{font-size:.8rem;font-weight:700;display:flex;align-items:center;gap:8px;position:relative;z-index:1;flex:1}.relay-status.on{color:var(--accent)}.relay-status.off{color:var(--text2)}.relay-dot{width:8px;height:8px;border-radius:50%;background:currentColor}.relay-status.on .relay-dot{box-shadow:0 0 12px currentColor;animation:pulse-dot 1.5s ease-in-out infinite}@keyframes pulse-dot{0%,100%{transform:scale(1);opacity:1}50%{transform:scale(1.3);opacity:.7}}.relay-row{display:flex;align-items:center;justify-content:space-between;margin-top:12px}.relay-row .toggle{width:52px;height:26px}.relay-row .toggle-track{border-width:1px}.relay-row .toggle-thumb{width:20px;height:20px}.relay-row .toggle input:checked~.toggle-track .toggle-thumb{transform:translateX(26px)}.sensor-section{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:20px}.sensor-card{background:linear-gradient(145deg,var(--surface),var(--surface2));border:1px solid var(--border);border-radius:16px;padding:24px;transition:all .3s ease;position:relative;overflow:hidden}.sensor-card:hover{transform:translateY(-2px)}.sensor-header{display:flex;justify-content:space-between;align-items:flex-start;margin-bottom:12px}.sensor-icon{font-size:1.6rem;animation:float 3s ease-in-out infinite}@keyframes float{0%,100%{transform:translateY(0)}50%{transform:translateY(-4px)}}.sensor-trend{font-size:1.2rem;transition:transform .3s}.sensor-card.hot .sensor-trend{color:#ff6b6b}.sensor-card.cold .sensor-trend{color:#74b9ff}.sensor-label{font-size:.7rem;font-family:'JetBrains Mono',monospace;color:var(--text2);text-transform:uppercase;letter-spacing:.12em;margin-bottom:8px}.sensor-value-row{display:flex;align-items:baseline;gap:6px}.sensor-value{font-size:2.4rem;font-weight:800;line-height:1;background:linear-gradient(90deg,var(--text),var(--accent));-webkit-background-clip:text;-webkit-text-fill-color:transparent}.sensor-unit{font-size:.9rem;color:var(--text2)}.sensor-source{font-size:.65rem;font-family:'JetBrains Mono',monospace;color:var(--text2);margin-top:12px;opacity:.7}footer{text-align:center;padding:24px;font-family:'JetBrains Mono',monospace;font-size:.65rem;color:var(--text2);border-top:1px solid var(--border);position:relative}footer::before{content:'';position:absolute;top:0;left:50%;transform:translateX(-50%);width:40px;height:2px;background:linear-gradient(90deg,transparent,var(--accent),transparent)}@media(max-width:768px){header{flex-direction:column;gap:16px;padding:16px}.status-bar{justify-content:center}.master-card{flex-direction:column;text-align:center}.master-toggle{flex-direction:column;gap:12px}.relay-grid{grid-template-columns:1fr}}</style></head><body><header><div class="logo"><div class="logo-dot" id="wifiDot"></div><h1>ESP32 <span>Automacao</span></h1></div><div class="status-bar"><div class="badge online" id="badgeWifi"><div class="badge-dot"></div> WiFi OK</div><div class="badge online" id="badgeMqtt"><div class="badge-dot"></div> MQTT OK</div></div></header><main><div class="info-bar"><div class="info-item"><label>Hardware</label><span>ESP32-WROOM-32</span></div><div class="info-item"><label>IP Fixo</label><span>192.168.15.160</span></div><div class="info-item"><label>MQTT Broker</label><span>192.168.15.150:1883</span></div><div class="info-item"><label>Core 0</label><span>WiFi + MQTT + OTA</span></div><div class="info-item"><label>Core 1</label><span>Sensores + Relés + Web</span></div><div class="info-item"><label>WebServer</label><span>Porta 8181</span></div></div><div><div class="section-label">Controle Geral</div><div class="master-card" onclick="document.getElementById('masterSwitch').click()"><div class="master-info"><h2>Todos os Reis</h2><p>QoS 2 - Retain true - 8 saidas active-low</p></div><div class="master-toggle"><span class="toggle-label" id="masterLabel">DESLIGADO</span><label class="toggle"><input type="checkbox" id="masterSwitch" onchange="toggleAll(this.checked)"><div class="toggle-track"><div class="toggle-thumb"></div></div></label></div></div></div><div><div class="section-label">Reis Individuais</div><div class="relay-grid" id="relayGrid"></div></div><div><div class="section-label">Monitoramento Ambiental</div><div class="sensor-section"><div class="sensor-card" id="sensor-dht"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#ff6b6b" stroke-width="2"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg></div><div class="sensor-trend" id="tempTrend">-</div></div><div class="sensor-label">Temperatura</div><div class="sensor-value-row"><span class="sensor-value" id="dhtTemp">--</span><span class="sensor-unit">C</span></div><div class="sensor-source">DHT22 - GPIO 25 - QoS 0</div></div><div class="sensor-card" id="sensor-hum"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#74b9ff" stroke-width="2"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg></div><div class="sensor-trend" id="humTrend">-</div></div><div class="sensor-label">Umidade</div><div class="sensor-value-row"><span class="sensor-value" id="dhtHum">--</span><span class="sensor-unit">%</span></div><div class="sensor-source">DHT22 - GPIO 25</div></div><div class="sensor-card" id="sensor-feel"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#ff6b6b" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M8 14s1.5 2 4 2 4-2 4-2"/><line x1="9" y1="9" x2="9.01" y2="9"/><line x1="15" y1="9" x2="15.01" y2="9"/></svg></div><div class="sensor-trend" id="feelTrend">-</div></div><div class="sensor-label">Sensacao Termica</div><div class="sensor-value-row"><span class="sensor-value" id="dhtFeel">--</span><span class="sensor-unit">C</span></div><div class="sensor-source">DHT22 - Heat Index</div></div><div class="sensor-card" id="sensor-dew"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#a29bfe" stroke-width="2"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg></div><div class="sensor-trend" id="dewTrend">-</div></div><div class="sensor-label">Ponto de Orvalho</div><div class="sensor-value-row"><span class="sensor-value" id="dhtDew">--</span><span class="sensor-unit">C</span></div><div class="sensor-source">DHT22 - Magnus</div></div><div class="sensor-card"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#a29bfe" stroke-width="2"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg></div><div class="sensor-trend" id="bmpTempTrend">-</div></div><div class="sensor-label">Temp. BMP180</div><div class="sensor-value-row"><span class="sensor-value" id="bmpTemp">--</span><span class="sensor-unit">C</span></div><div class="sensor-source">BMP180 - I2C SDA:21 SCL:22</div></div><div class="sensor-card"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#00b894" stroke-width="2"><path d="M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2"/></svg></div><div class="sensor-trend" id="pressTrend">-</div></div><div class="sensor-label">Pressao Atmosferica</div><div class="sensor-value-row"><span class="sensor-value" id="bmpPress">--</span><span class="sensor-unit">hPa</span></div><div class="sensor-source">BMP180 - Ref: 1012 hPa</div></div><div class="sensor-card"><div class="sensor-header"><div class="sensor-icon"><svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#fdcb6e" stroke-width="2"><path d="M8 3v3a2 2 0 0 1-2 2H3m18 0h-3a2 2 0 0 1-2-2V3m0 18v-3a2 2 0 0 1 2-2h3M3 16h3a2 2 0 0 1 2 2v3"/></svg></div><div class="sensor-trend" id="altTrend">-</div></div><div class="sensor-label">Altitude Real</div><div class="sensor-value-row"><span class="sensor-value" id="bmpAlt">--</span><span class="sensor-unit">m</span></div><div class="sensor-source">BMP180 - Base: 92m (Manaus)</div></div></div></div></main><footer>ESP32 Sistema IoT - Robson Brasil - Dual-Core FreeRTOS - MQTT - ArduinoOTA - WebServer | Tópicos: ESP32/MinhaCasa/QuartoRobson/...</footer><script>const RELAYS=[{name:'Rei 1 - Luz Quarto',gpio:26,state:false},{name:'Rei 2 - Luz Sala',gpio:13,state:false},{name:'Rei 3 - Ventilador',gpio:14,state:false},{name:'Rei 4 - Tomada 1',gpio:32,state:false},{name:'Rei 5 - Tomada 2',gpio:16,state:false},{name:'Rei 6 - Luz Cozinha',gpio:17,state:false},{name:'Rei 7 - Ar Condicionado',gpio:18,state:false},{name:'Rei 8 - Portao',gpio:19,state:false}];function renderRelays(){const g=document.getElementById('relayGrid');g.innerHTML='';RELAYS.forEach((r,i)=>{const c=document.createElement('div');c.className='relay-card'+(r.state?' active':'');c.id='relay-card-'+i;c.innerHTML='<div class="relay-header"><span class="relay-name" title="Clique para renomear" onclick="renameRelay('+i+')">'+r.name+'</span></div><div class="relay-row"><div class="relay-status '+(r.state?'on':'off')+'" id="relay-status-'+i+'"><span class="relay-dot"></span>'+(r.state?'LIGADO':'DESLIGADO')+'</div><label class="toggle"><input type="checkbox" '+(r.state?'checked':'')+' onchange="toggleRelay('+i+',this.checked)"><div class="toggle-track"><div class="toggle-thumb"></div></div></label></div>';g.appendChild(c)})}function renameRelay(i){const n=prompt('Digite o novo nome:',RELAYS[i].name);if(n!==null&&n.trim()!==''){RELAYS[i].name=n.trim();renderRelays();const f=new FormData();f.append('name',n.trim());fetch('/api/setname/'+i,{method:'POST',body:f})}}function toggleRelay(i,s){RELAYS[i].state=s;const c=document.getElementById('relay-card-'+i),st=document.getElementById('relay-status-'+i);c.className='relay-card'+(s?' active':'');st.className='relay-status '+(s?'on':'off');st.innerHTML='<span class="relay-dot"></span>'+(s?'LIGADO':'DESLIGADO');fetch('/api/relay/'+i+'/'+(s?1:0),{method:'POST'});}function toggleAll(s){const l=document.getElementById('masterLabel');l.textContent=s?'LIGADO':'DESLIGADO';l.classList.toggle('active',s);RELAYS.forEach((_,i)=>{RELAYS[i].state=s;const c=document.getElementById('relay-card-'+i),st=document.getElementById('relay-status-'+i),ch=c.querySelector('input');if(c&&st&&ch){ch.checked=s;c.className='relay-card'+(s?' active':'');st.className='relay-status '+(s?'on':'off');st.innerHTML='<span class="relay-dot"></span>'+(s?'LIGADO':'DESLIGADO')}});fetch('/api/all/'+(s?1:0),{method:'POST'});}function updateTrend(id,n,o){const e=document.getElementById(id);if(n>o+0.5){e.textContent='↑';e.style.color='#ff6b6b'}else if(n<o-0.5){e.textContent='↓';e.style.color='#74b9ff'}else{e.textContent='→';e.style.color='#8892a8'}}function updateSensors(d){const ot=parseFloat(document.getElementById('dhtTemp').textContent)||24,oh=parseFloat(document.getElementById('dhtHum').textContent)||60;document.getElementById('dhtTemp').textContent=d.temp;document.getElementById('dhtHum').textContent=d.hum;document.getElementById('dhtFeel').textContent=d.tempFeel;document.getElementById('dhtDew').textContent=d.dew;document.getElementById('bmpTemp').textContent=d.bmpTemp;document.getElementById('bmpPress').textContent=d.press;document.getElementById('bmpAlt').textContent=d.alt;updateTrend('tempTrend',parseFloat(d.temp),ot);updateTrend('humTrend',parseFloat(d.hum),oh)}async function load(){try{const r=await fetch('/api/relays');const data=await r.json();data.states.forEach((s,i)=>{if(RELAYS[i])RELAYS[i].state=!!s});data.names.forEach((n,i)=>{if(RELAYS[i]&&n)RELAYS[i].name=n});const s=await fetch('/api/sensors').then(r=>r.json());updateSensors(s);document.getElementById('badgeMqtt').className='badge online';document.getElementById('badgeMqtt').innerHTML='<div class="badge-dot"></div> MQTT OK'}catch(e){document.getElementById('badgeMqtt').className='badge offline';document.getElementById('badgeMqtt').innerHTML='<div class="badge-dot"></div> MQTT OFF'}renderRelays()}load();setInterval(load,10000);</script></body></html>
)rawl";

WebServer webServer(8181);

void handleRoot() {
    webServer.send_P(200, "text/html", DASHBOARD_HTML);
}

void handleApiRelays() {
    char json[512];
    snprintf(json, sizeof(json), "{\"states\":[%d,%d,%d,%d,%d,%d,%d,%d],\"names\":[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"]}", 
        RelayState1, RelayState2, RelayState3, RelayState4,
        RelayState5, RelayState6, RelayState7, RelayState8,
        getRelayName(0).c_str(), getRelayName(1).c_str(), getRelayName(2).c_str(),
        getRelayName(3).c_str(), getRelayName(4).c_str(), getRelayName(5).c_str(),
        getRelayName(6).c_str(), getRelayName(7).c_str());
    webServer.send(200, "application/json", json);
}

void handleApiSensors() {
    char json[512];
    snprintf(json, sizeof(json), "{\"temp\":\"%s\",\"hum\":\"%s\",\"press\":\"%.1f\",\"tempFeel\":\"%s\",\"dew\":\"%s\",\"bmpTemp\":\"%.1f\",\"alt\":\"%.1f\",\"mqtt\":%s}",
        str_temp_data, str_hum_data, pressure/100.0, str_tempterm_data, str_dewpoint_data, temperature, altitude, mqttConnected ? "true" : "false");
    webServer.send(200, "application/json", json);
}

void handleApiRelay() {
    String uri = webServer.uri();
    int idx, state;
    if (sscanf(uri.c_str(), "/api/relay/%d/%d", &idx, &state) == 2) {
        bool bState = (state == 1);
        if (idx >= 0 && idx < 8) {
            const int relayPins[8] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
            digitalWrite(relayPins[idx], bState ? LOW : HIGH);
            logRelayAction("Dashboard", idx, bState);

            bool* relayStates[] = {&RelayState1, &RelayState2, &RelayState3, &RelayState4,
                                 &RelayState5, &RelayState6, &RelayState7, &RelayState8};
            *relayStates[idx] = bState;

            saveRelayState(idx + 1, bState);
            pendingMqttUpdate = true; // Sinaliza para Core 0 enviar ao MQTT
            pendingSinricProUpdate = true; // Sinaliza para Core 0 enviar ao SinricPro
            webServer.send(200, "text/plain", "OK");
            notifyTelegramStateChange("Dashboard", idx, bState); // Agora é assíncrono
            return;
        }
    }
    webServer.send(400, "text/plain", "Error");
}

void handleApiAll() {
    String uri = webServer.uri();
    int state;
    if (sscanf(uri.c_str(), "/api/all/%d", &state) == 1) {
        bool bState = (state == 1);
        Todos = bState;

        const int relayPins[8] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
        for (int i = 0; i < 8; i++) {
            digitalWrite(relayPins[i], bState ? LOW : HIGH);
        }
        logRelayAction("Dashboard", -1, bState);

        bool* relayStates[] = {&RelayState1, &RelayState2, &RelayState3, &RelayState4,
                             &RelayState5, &RelayState6, &RelayState7, &RelayState8};
        for (int i = 0; i < 8; i++) {
            *relayStates[i] = bState;
            saveRelayState(i + 1, bState);
        }

        pendingMqttUpdate = true; // Sinaliza para Core 0 enviar ao MQTT
        pendingSinricProUpdate = true; // Sinaliza para Core 0 enviar ao SinricPro
        webServer.send(200, "text/plain", "OK");
        notifyTelegramStateChange("Dashboard", -1, bState);
        return;
    }
    webServer.send(400, "text/plain", "Error");
}

void handleApiSetName() {
    String uri = webServer.uri();
    Serial.print("[SETNAME] URI: ");
    Serial.println(uri);
    
    int idx = -1;
    String newName = "";
    
    // O prefixo "/api/setname/" tem 13 caracteres.
    // Exemplo esperado: /api/setname/0 ou /api/setname/0/Nome
    if (uri.startsWith("/api/setname/")) {
        String pathPart = uri.substring(13); // Remove "/api/setname/"
        
        int slashPos = pathPart.indexOf('/');
        if (slashPos != -1) {
            // Formato: /api/setname/0/Nome
            idx = pathPart.substring(0, slashPos).toInt();
            newName = pathPart.substring(slashPos + 1);
        } else {
            // Formato: /api/setname/0 (nome no body ou argumento)
            idx = pathPart.toInt();
            if (webServer.hasArg("name")) {
                newName = webServer.arg("name");
            } else if (webServer.args() > 0) {
                newName = webServer.arg(0);
            }
        }
    }
    
    Serial.print("[SETNAME] IDX: ");
    Serial.println(idx);
    Serial.print("[SETNAME] NOME: ");
    Serial.println(newName);
    
    if (idx >= 0 && idx < 8 && newName.length() > 0) {
        newName = webServer.urlDecode(newName);
        
        Preferences p;
        char key[16];
        snprintf(key, sizeof(key), "RelayName%d", idx + 1);
        p.begin("app", false);
        p.putString(key, newName);
        p.end();
        
        Serial.print("[SETNAME] OK: ");
        Serial.println(key);
        webServer.send(200, "text/plain", "OK");
        return;
    }
    Serial.println("[SETNAME] ERRO NO PARSING");
    webServer.send(400, "text/plain", "Error");
}

void setupWebServer() {
    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/api/relays", HTTP_GET, handleApiRelays);
    webServer.on("/api/sensors", HTTP_GET, handleApiSensors);
    
    webServer.onNotFound([]() {
        String uri = webServer.uri();
        if (uri.startsWith("/api/relay/")) {
            handleApiRelay();
        } else if (uri.startsWith("/api/all/")) {
            handleApiAll();
        } else if (uri.startsWith("/api/setname/")) {
            handleApiSetName();
        } else {
            webServer.send(404, "text/plain", "Not Found");
        }
    });

    webServer.begin();
    Serial.println(F("[WEB] Servidor OK"));
}

void handleWebServerClient() {
    webServer.handleClient();
}

#endif // WEBSERVER_CONFIG_H