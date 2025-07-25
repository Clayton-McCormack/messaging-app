const { useState, useEffect } = React;

function App() {
  const [messages, setMessages] = useState([]);
  const [username, setUsername] = useState('');
  const [content, setContent] = useState('');

  const fetchMessages = async () => {
    try {
      const res = await fetch('http://localhost:8080/messages');
      const data = await res.json();
      setMessages(data);
    } catch (err) {
      console.error(err);
    }
  };

  const sendMessage = async () => {
    try {
      await fetch('http://localhost:8080/messages', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, content })
      });
      setContent('');
      fetchMessages();
    } catch (err) {
      console.error(err);
    }
  };

  useEffect(() => {
    fetchMessages();
  }, []);

  return (
    <div>
      <div id="messages">
        {messages.map((m, idx) => (
          <div key={idx} className="message">
            <strong>{m.username}:</strong> {m.content}
          </div>
        ))}
      </div>
      <input placeholder="Name" value={username} onChange={e => setUsername(e.target.value)} />
      <br />
      <textarea placeholder="Message" value={content} onChange={e => setContent(e.target.value)} />
      <br />
      <button onClick={sendMessage}>Send</button>
    </div>
  );
}

ReactDOM.createRoot(document.getElementById('root')).render(<App />);
