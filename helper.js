
function ompleLlistes() {
  escriuAccions("output/stocks_slope_percent_", "llista", "csv");
  escriuAccions("output/stocks_sorted_stats_", "llista2", "txt");
}

function escriuAccions(filenameSufix, elementId, ext) {
  const list = document.getElementById(elementId);
  if (!list) return;

  for (let i = 0; i < 15; i++) {
    let data = new Date();
    var dateOffset = (24 * 60 * 60 * 1000) * i;
    data.setTime(data.getTime() - dateOffset);

    if (data.getDay() === 0 || data.getDay() === 6) continue; // Skip weekends

    const dia = formatejaData(data);
    const nom = dia;
    const filename = filenameSufix + dia + "." + ext;

    const diesSetmana = ["Diumenge", "Dilluns", "Dimarts", "Dimecres", "Dijous", "Divendres", "Dissabte"];
    const nomDia = diesSetmana[data.getDay()];

    const card = document.createElement('div');
    card.className = 'card';
    card.onclick = () => window.location.href = `view_report.html?file=${filename}`;
    card.innerHTML = `
            <span class="card-title">${nomDia}, ${nom}</span>
            <span class="card-meta">Prem per veure l'anàlisi</span>
        `;
    list.appendChild(card);
  }
}

function formatejaData(data) {
  const offset = data.getTimezoneOffset();
  const d = new Date(data.getTime() - (offset * 60 * 1000));
  return d.toISOString().split('T')[0];
}

async function renderReportPage(fileName) {
  const titleEl = document.getElementById('report-title');
  const metaEl = document.getElementById('report-meta');
  const statusEl = document.getElementById('loading-status');

  try {
    const displayTitle = fileName.split('/').pop()
      .replace('.txt', '').replace('.csv', '')
      .replace(/_/g, ' ');

    titleEl.innerText = displayTitle;
    metaEl.innerText = `Fitxer: ${fileName}`;

    // Cache busting to ensure we get the latest data
    const response = await fetch(fileName + '?t=' + new Date().getTime());
    if (!response.ok) throw new Error("File not found");
    const text = await response.text();

    const isStats = fileName.includes('sorted_stats');
    const data = isStats ? parseStatsReport(text) : parseCsvReport(text);

    renderTable(data);
    statusEl.style.display = 'none';
  } catch (err) {
    titleEl.innerText = "Error carregant l'informe";
    metaEl.innerText = `No s'ha pogut trobar o processar el fitxer: ${fileName}`;
    statusEl.innerText = "Error";
    statusEl.className = "badge badge-negative";
    console.error(err);
  }
}

function parseCsvReport(text) {
  const lines = text.trim().split('\n');
  const headers = ["Percentil", "Pendent", "Volatilitat", "Guany Max", "Perdua Max", "Ticker"];
  const rows = [];

  // Skip potential header lines in the CSV if they exist
  lines.forEach(line => {
    const parts = line.split(',');
    if (parts.length >= 6 && !isNaN(parseFloat(parts[0]))) {
      rows.push({
        values: parts.map(p => p.trim()),
        isCategory: false
      });
    }
  });

  return { headers, rows };
}

function normalizeLabel(label) {
  let l = label.toLowerCase()
    .replace('showing ', '')
    .replace('size ', '')
    .replace(/:$/, '')
    .trim();

  // Map old labels to new ones
  if (l === 'slope_00') return '0.0 <= slope < 0.1';
  if (l === 'slope_01') return '0.1 <= slope < 0.2';
  if (l === 'slope_02') return '0.2 <= slope < 0.3';
  if (l === 'slope_03') return 'slope >= 0.3';

  // Ensure consistent decimal points
  return l.replace('< 0', '< 0.0');
}

function parseStatsReport(text) {
  const lines = text.split('\n');
  const headers = ["%", "Pendent", "Volat.", "Guany", "Perdua", "Ticker"];
  const rows = [];
  const summary = [];
  const seenCategories = new Set();

  lines.forEach(line => {
    let trimmed = line.trim();
    if (!trimmed) return;

    // Extract summary counts
    if (trimmed.toLowerCase().startsWith('size ')) {
      const parts = trimmed.split(':');
      if (parts.length === 2) {
        const label = normalizeLabel(parts[0]);
        summary.push({
          label: label,
          count: parts[1].trim()
        });
      }
      return;
    }

    // Filter out metadata
    if (trimmed.toLowerCase().startsWith('output format') ||
      trimmed.includes('% - yearly') ||
      trimmed.includes('percentile(%)') ||
      trimmed.includes('yearly slope')) {
      return;
    }

    if (trimmed.startsWith('showing ')) {
      if (trimmed.includes('best stocks')) return;
      const label = "showing " + normalizeLabel(trimmed) + ":";

      // Only add category if we haven't seen it yet (prevents duplicates from messy files)
      if (!seenCategories.has(label)) {
        rows.push({ name: label, isCategory: true });
        seenCategories.add(label);
      }
    } else if (trimmed.includes(' - ')) {
      const parts = trimmed.split(' - ').map(p => p.trim());
      if (parts.length >= 6) {
        rows.push({
          values: parts,
          isCategory: false
        });
      }
    }
  });

  return { headers, rows, summary };
}

let currentTableData = null;

function renderTable(data) {
  currentTableData = data;

  // Render Summary
  const summaryEl = document.getElementById('report-summary');
  if (summaryEl && data.summary && data.summary.length > 0) {
    summaryEl.innerHTML = data.summary.map(s => `
            <div class="card" style="padding: 1rem; cursor: default;">
                <span class="card-meta" style="font-size: 0.7rem; text-transform: uppercase;">${s.label}</span>
                <span class="card-title" style="margin: 0; font-size: 1.5rem; color: var(--primary);">${s.count}</span>
            </div>
        `).join('');
  }

  const head = document.getElementById('table-head');
  const body = document.getElementById('table-body');

  head.innerHTML = `<tr>${data.headers.map((h, i) => `<th onclick="sortTable(${i})">${h}</th>`).join('')}</tr>`;

  renderRows(data.rows);
}

function renderRows(rows) {
  const body = document.getElementById('table-body');
  body.innerHTML = '';

  rows.forEach(row => {
    const tr = document.createElement('tr');
    if (row.isCategory) {
      tr.innerHTML = `<td colspan="${currentTableData.headers.length}" class="category-header">${row.name}</td>`;
    } else {
      tr.innerHTML = row.values.map((v, i) => {
        let cellBody = v;
        if (i === 1) { // Slope coloring
          const val = parseFloat(v);
          const colorClass = val > 0.1 ? 'badge-positive' : (val < 0 ? 'badge-negative' : '');
          cellBody = `<span class="badge ${colorClass}">${v}</span>`;
        }
        return `<td>${cellBody}</td>`;
      }).join('');
    }
    body.appendChild(tr);
  });
}

function sortTable(colIndex) {
  // Only sort data rows, keep categories in place? 
  // Usually it's better to flatten or just sort within categories.
  // For simplicity, let's flatten data and sort all stocks together if requested

  const dataRows = currentTableData.rows.filter(r => !r.isCategory);
  const isNumeric = !isNaN(parseFloat(dataRows[0].values[colIndex]));

  dataRows.sort((a, b) => {
    let valA = a.values[colIndex];
    let valB = b.values[colIndex];

    if (isNumeric) {
      return parseFloat(valB) - parseFloat(valA); // Default Descending
    }
    return valA.localeCompare(valB);
  });

  // When sorting, categories might lose meaning, so we show a "Sorted View"
  renderRows([
    { name: "Resultats Ordenats (Vista sense categories)", isCategory: true },
    ...dataRows
  ]);
}