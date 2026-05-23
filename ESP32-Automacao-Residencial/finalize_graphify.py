#!/usr/bin/env python3
import json
from graphify.build import build_from_json
from graphify.export import to_html, to_svg
from pathlib import Path

# Carregar dados
extraction = json.loads(Path('.graphify_extract.json').read_text())
analysis = json.loads(Path('.graphify_analysis.json').read_text())

# Rótulos das comunidades (baseado nos "God Nodes")
label_map = {
    0: "Automação MQTT (ESP32)",
    1: "Integração Telegram",
    2: "Configuração Alexa",
    3: "Scripts Python (Skills)",
    4: "Documentação"  # Node_modules + SKILL.md
}

# Atualizar relatório
report = Path('graphify-out/GRAPH_REPORT.md').read_text()
for cid, label in label_map.items():
    report = report.replace(f'Comunidade {cid}', label)
Path('graphify-out/GRAPH_REPORT.md').write_text(report)

# Gerar HTML/SVG
G = build_from_json(extraction)
communities = {int(k): v for k, v in analysis['communities'].items()}

to_html(G, communities, 'graphify-out/graph.html', community_labels=label_map)
to_svg(G, communities, 'graphify-out/graph.svg', community_labels=label_map)

print('✅ Sucesso, Senhor!')
print('Arquivos prontos em graphify-out/')
print('- graph.html (abre no navegador)')
print('- GRAPH_REPORT.md (relatório técnico)')
print('- graph.svg (visualização)')