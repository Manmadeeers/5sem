document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('loadJson').addEventListener('click', async () => {
    const out = document.getElementById('jsonOut');
    try {
      const res = await fetch('/data.json');
      if (!res.ok) throw new Error('HTTP ' + res.status);
      const data = await res.json();
      out.textContent = JSON.stringify(data, null, 2);
    } catch (e) {
      out.textContent = 'Error: ' + e.message;
    }
  });

  document.getElementById('loadXml').addEventListener('click', async () => {
    const out = document.getElementById('xmlOut');
    try {
      const res = await fetch('/data.xml');
      if (!res.ok) throw new Error('HTTP ' + res.status);
      const txt = await res.text();
      out.textContent = txt;
    } catch (e) {
      out.textContent = 'Error: ' + e.message;
    }
  });
});