
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
  const headers = ["Percentil", "Slope 2y", "Slope 6m", "Slope 1m", "Slope 6d", "Volatilitat", "Guany esperat", "Perdua esperada", "RSI14", "RelVol30", "Ticker"];
  const rows = [];

  // Skip potential header lines in the CSV if they exist
  lines.forEach(line => {
    let trimmed = line.trim();
    if (!trimmed) return;
    const parts = trimmed.split(',');
    if (parts.length >= 11 && !isNaN(parseFloat(parts[0]))) {
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
  const headers = ["Percentil", "Pendent 2y", "Pendent 6m", "Slope 1m", "Slope 6d", "Volat.", "Guany esperat", "Perdua esperada", "RSI14", "RelVol30", "Ticker"];
  const groups = [];
  const summary = [];
  let currentGroup = null;

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

      currentGroup = {
        name: label,
        rows: [],
        visibleCount: 10
      };
      groups.push(currentGroup);
    } else if (trimmed.includes(' - ') || (trimmed.includes(',') && trimmed.split(',').length >= 11)) {
      const sep = trimmed.includes(' - ') ? ' - ' : ',';
      const parts = trimmed.split(sep).map(p => p.trim());
      if (parts.length >= 11) {
        if (!currentGroup) {
          currentGroup = { name: "Altres", rows: [], visibleCount: 10 };
          groups.push(currentGroup);
        }
        currentGroup.rows.push({
          values: parts,
          isCategory: false
        });
      }
    }
  });

  return { headers, groups, summary };
}

let currentTableData = null;

function renderTable(data) {
  // If no groups, create a default group for flat CSV data
  if (!data.groups && data.rows) {
    data.groups = [{
      name: "Llista d'accions",
      rows: data.rows,
      visibleCount: 50 // Show more by default for flat lists
    }];
  }
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
  head.innerHTML = `<tr>${data.headers.map((h, i) => `<th onclick="sortTable(${i})">${h}</th>`).join('')}</tr>`;

  renderGroups();
}

function renderGroups() {
  const body = document.getElementById('table-body');
  body.innerHTML = '';

  currentTableData.groups.forEach((group, groupIdx) => {
    // Skip empty groups unless they are special
    if (group.rows.length === 0 && !group.name.includes("showing")) return;

    // Category Header Row
    const headerTr = document.createElement('tr');
    headerTr.innerHTML = `<td colspan="${currentTableData.headers.length}" class="category-header">${group.name} (${group.rows.length} accions)</td>`;
    body.appendChild(headerTr);

    // Data Rows
    const visibleRows = group.rows.slice(0, group.visibleCount);
    visibleRows.forEach(row => {
      const tr = document.createElement('tr');
      tr.innerHTML = row.values.map((v, i) => {
        let cellBody = v;
        const val = parseFloat(v);

        // Add % to Percentile, Volatility, Gain, Loss, RSI14
        if (i === 0 || i === 5 || i === 6 || i === 7 || i === 8) {
          cellBody = `${v}%`;
        }

        if (i >= 1 && i <= 4) { // Slope coloring: Green > 0.1, Red < -0.1
          const colorClass = val > 0.1 ? 'badge-positive' : (val < -0.1 ? 'badge-negative' : '');
          cellBody = `<span class="badge ${colorClass}">${v}</span>`;
        } else if (i === 6) { // Guany esperat coloring: Green > 50
          if (val > 50) {
            cellBody = `<span class="badge badge-positive">${v}%</span>`;
          }
        } else if (i === 7) { // Perdua esperada coloring: Red > 50 (abs)
          if (Math.abs(val) > 50) {
            cellBody = `<span class="badge badge-negative">${v}%</span>`;
          }
        } else if (i === 8) { // RSI14 coloring: Red > 70, Green < 30
          const colorClass = val > 70 ? 'badge-negative' : (val < 30 && val !== -1 ? 'badge-positive' : '');
          cellBody = `<span class="badge ${colorClass}">${v}</span>`;
        } else if (i === 9) { // RelVol30 coloring: Green > 2
          const colorClass = val > 2 ? 'badge-positive' : '';
          cellBody = `<span class="badge ${colorClass}">${v}</span>`;
        }
        return `<td>${cellBody}</td>`;
      }).join('');
      body.appendChild(tr);
    });

    // Show More Row
    if (group.rows.length > group.visibleCount) {
      const moreTr = document.createElement('tr');
      moreTr.innerHTML = `
                <td colspan="${currentTableData.headers.length}" style="text-align: center; padding: 1rem;">
                    <button class="badge" style="background: var(--primary); color: white; border: none; cursor: pointer; padding: 0.5rem 1.5rem;" onclick="showMore(${groupIdx})">
                        Mostra més accions (+10)
                    </button>
                </td>
            `;
      body.appendChild(moreTr);
    }
  });
}

function showMore(groupIdx) {
  currentTableData.groups[groupIdx].visibleCount += 10;
  renderGroups();
}

function sortTable(colIndex) {
  // Sort within each group
  currentTableData.groups.forEach(group => {
    if (group.rows.length === 0) return;
    const isNumeric = !isNaN(parseFloat(group.rows[0].values[colIndex]));

    group.rows.sort((a, b) => {
      let valA = a.values[colIndex];
      let valB = b.values[colIndex];
      if (isNumeric) {
        // RSI14 is at index 8. User wants it sorted from small to large.
        if (colIndex === 8) {
          return parseFloat(valA) - parseFloat(valB);
        }
        return parseFloat(valB) - parseFloat(valA);
      }
      return valA.localeCompare(valB);
    });
  });
  renderGroups();
}