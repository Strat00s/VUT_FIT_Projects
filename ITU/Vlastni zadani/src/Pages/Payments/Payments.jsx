import './Payments.css';
import React from 'react';

import Container from 'react-bootstrap/Container';
import Row from 'react-bootstrap/Row';
import Col from 'react-bootstrap/Col';
import Card from 'react-bootstrap/Card';
import Form from 'react-bootstrap/Form';
import Dropdown from 'react-bootstrap/Dropdown';
import DropdownButton from 'react-bootstrap/DropdownButton';
import Button from 'react-bootstrap/Button'
import InputGroup from 'react-bootstrap/InputGroup';
import Modal from 'react-bootstrap/Modal';

import { useState } from 'react';
import axios from 'axios';

import TopBar from '../../Components/TopBar'

function Payments(props) {
    var data = props.location.data;

    const [to_user, set_to_user] = useState('');
    const [amount, set_amount] = useState(0);
    const [symbol, set_symbol] = useState(data.balances[0].symbol);
    const [note, set_note] = useState('');

    //Error popup
    const [errorPopup, setErrorPopup] = useState(false);
    const [errorMessage, setErrorMessage] = useState('');
    const [errorStyle, setErrorStyle] = useState('');
    const setError = (message, is_on, style) => {
        setErrorMessage(message);
        setErrorPopup(is_on);
        setErrorStyle(style);
    }

    const handleEPopupClose = () => setErrorPopup(false);   //error handle

    const onToChange = (e) => set_to_user(e.target.value);
    const onAmountChange = (e) => set_amount(parseInt(e.target.value));
    const onSymbolChange = (e) => set_symbol(e);
    const onNoteChange = (e) => set_note(e.target.value);

    const sendPayment = async() => {
        try {
            await axios.post("http://"+data.ip+"/currencies/"+symbol+"/transactions", {'recipient': to_user, 'amount': amount, 'note': note}, {headers: {"Content-Type": "application/json", "Authorization": "Bearer "+data.token}});
            setError("Payment sent succesfully", true, "success");
        }
        catch(error){
            if (error.response === undefined) {
                setError("Server is unavailable", true, "warning");
            }
            else if (error.response.data.errors){
                setError("Invalid amount", true, "warning");
            }
            else if (error.response.data.data.recipient) {
                setError(error.response.data.data.recipient, true, "warning");
            }
            else if (error.response.data.data.balance) {
                setError(error.response.data.data.balance, true, "warning");
            }
        }
    }

    const getSymbols = data.balances.map((balance) =>
        <Dropdown.Item eventKey={balance.symbol}>{balance.symbol}</Dropdown.Item>
    );

    return (
        <div className="PaymentPage">
            <Modal className="rounded" size="sm" show={errorPopup} onHide={handleEPopupClose}>
                <Container className="bg-dark text-white border-0">
                    <div className="LPErrorModalMessage">
                        <Modal.Header>{errorMessage}</Modal.Header>
                    </div>
                </Container>
                <Modal.Footer className=" border-0 bg-dark text-white">
                    <Container>
                        <div className="LPErrorModalButton">
                            <Button variant={errorStyle} onClick={handleEPopupClose}>Close</Button>
                        </div>
                    </Container>
                </Modal.Footer>
            </Modal>

            <div className="PPHeader">
                <TopBar page_num={3} data={data}/>
            </div>

            <div className="PPMainBody">
                <Container>
                    <Card className="bg-dark text-light border border-light">
                        <Card.Header className="border-right-0 border-left-0 border-top-0 border border-light" 
                                     style={{'textAlign': 'center'}} 
                                     as="h4"
                        >
                            Send payments
                        </Card.Header>
                        <Card.Body> 
                            <Form>
                                <Form.Group as={Row} controlId="formPayFrom">
                                    <Form.Label column sm="2">
                                        From:
                                    </Form.Label>
                                    <Col sm="10">
                                        <Form.Control className="text-light" plaintext readOnly defaultValue={data.user_data.username}/>
                                    </Col>
                                </Form.Group>
                                <hr className="bg-light"/>
                                <Form.Group as={Row} controlId="formPayTo">
                                    <Form.Label column sm="2">
                                        To:
                                    </Form.Label>
                                    <Col>
                                        <Form.Control className="bg-dark text-light" type="username" placeholder="Recipient's username" onChange={onToChange} value={to_user}/>
                                    </Col>
                                </Form.Group>
                                <hr className="bg-light"/>
                                <Form.Group as={Row} controlId="formPayAmount">
                                    <Form.Label column sm="2">
                                        Amount:
                                    </Form.Label>
                                    <Col>
                                        <InputGroup>
                                            <Form.Control className="bg-dark text-light" type="number" onChange={onAmountChange} value={amount}/>
                                            <DropdownButton as={InputGroup.Append} menuAlign="right" variant="secondary" title={symbol} onSelect={onSymbolChange}>
                                                {getSymbols}
                                            </DropdownButton>
                                        </InputGroup>
                                        <Form.Text className="text-light" style={{'fontSize': 12}}>
                                            Higher than 0
                                        </Form.Text>
                                    </Col>
                                </Form.Group>
                                <hr className="bg-light"/>
                                <Form.Group as={Row} controlId="formNote">
                                    <Form.Label column sm="2">
                                        Note:
                                    </Form.Label>
                                    <Col>
                                        <Form.Control className="bg-dark text-light" type="note" placeholder="Short note" onChange={onNoteChange} value={note}/>
                                    </Col>
                                </Form.Group>
                                <hr className="bg-light"/>
                            </Form>
                            <div className="PPSendButton" >
                                <div style={{'minWidth': 150}}>
                                    <Button variant="primary" type="Submit" onClick={() => sendPayment()} block>Send</Button>
                                </div>
                            </div>
                        </Card.Body>
                    </Card>
                </Container>
            </div>
        </div>
    )
}

export default Payments