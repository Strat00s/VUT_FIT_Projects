import React from 'react';

import Card from 'react-bootstrap/Card';
import Container from 'react-bootstrap/Container';
import Row from 'react-bootstrap/Row';
import Col from 'react-bootstrap/Col';
import Dropdown from 'react-bootstrap/Dropdown';
import DropdownButton from 'react-bootstrap/DropdownButton';
import Button from 'react-bootstrap/Button';
import Listgroup from 'react-bootstrap/ListGroup';
import Modal from 'react-bootstrap/Modal';

import { useState, useEffect } from 'react';
import axios from 'axios';
import { Line } from "react-chartjs-2";

import TopBar from '../../Components/TopBar';

//TODO comments

function History(props) {
    var data = props.location.data;

    //const [symbol, setSymbol] = useState(data.last_symbol);  //current symbol
    const [historyType, setHistoryType] = useState(false);          //change between graph and table view
    const [refresh, setRefresh] = useState(false);                  //force refresh once we get new history data

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

    //update the actual history
    const updateHistory = async(curr_symbol, want_refresh) => {
        try {
            let response = await axios.get("http://"+data.ip+"/currencies/"+curr_symbol+"/transactions/"+data.user_data.username, {headers: {"Content-Type": "application/json", "Authorization": "Bearer "+data.token}});
            data.history = response.data;
            if (want_refresh === true) setRefresh(!refresh);
        }
        catch(error) { 
            setError("Server is unavailable", true, "warning");
        }
    }

    useEffect(() => {
        updateHistory(data.last_symbol, false);
    })

    //update history data
    const onSymbolChange = (e) => {
        data.last_symbol = e; //setSymbol(e);
        updateHistory(e, true);
    }
    const onTypeChange = () => setHistoryType(!historyType);    //flip history type

    //draw dropdown items for every currency
    const getSymbols = data.balances.map((balance) =>
        <Dropdown.Item eventKey={balance.symbol}>{balance.symbol}</Dropdown.Item>
    );

    const showHistory = data.history.map((single_history) =>
        <div>
            <Card className="bg-secondary text-light border-0">
                {data.user_data.username === single_history.sender_username &&    <Listgroup.Item className="bg-dark text-light border-danger" > <Row> <Col>{single_history.transaction_time}</Col> <Col> - {single_history.amount}</Col> <Col>{single_history.symbol}</Col> <Col>{single_history.note === "" ? "----------------" : single_history.note}</Col> </Row> </Listgroup.Item>}
                {data.user_data.username === single_history.recipient_username && <Listgroup.Item className="bg-dark text-light border-success"> <Row> <Col>{single_history.transaction_time}</Col> <Col> + {single_history.amount}</Col> <Col>{single_history.symbol}</Col> <Col>{single_history.note === "" ? "----------------" : single_history.note}</Col> </Row> </Listgroup.Item>}
            </Card>
            <hr className="border-light"/>
        </div>
    );

    //calculate amount change
    const getGraphHistory = (curr_symbol) => {
        var reversed_history = data.history.slice().reverse();
        var chart_data = {
            labels: [],
            datasets: [{ 
                label: curr_symbol,
                data: [],
                borderColor: ["#f7f7f7"],
                pointBackgroundColor: [],
                pointRadius: 6,
                pointHitRadius: 12,
                borderWidth: 3,
                }]
            };
        var amount = 0;
        for (let i = 0; i < reversed_history.length; i++) {
            chart_data.labels.push(reversed_history[i].transaction_time);
            if (reversed_history[i].sender_username === data.user_data.username) {
                amount = amount - reversed_history[i].amount;
                chart_data.datasets[0].pointBackgroundColor.push('#d9534f')
            }
            else {
                amount = amount + reversed_history[i].amount;
                chart_data.datasets[0].pointBackgroundColor.push('#5cb85c')
            }
            chart_data.datasets[0].data.push(amount);
        }
        return chart_data;
    }

    return (
        <div>
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
            <div className="HIPHeader">
                <TopBar page_num={2} data={data}/>
            </div>
            <div className="HIPMainBody">
            <Container>
                <Card className="bg-dark text-light border border-light">
                    <Card.Header className="border-right-0 border-left-0 border-top-0 border border-light text-light"
                                 style={{'textAlign': 'center'}}
                                 as="h4">
                        <Row>
                            <Col xs={4}>
                            <Button style={{"float" : "left", "fontSize" : 14}} onClick={onTypeChange}>
                                {historyType === false && <div>Account history</div>}
                                {historyType === true && <div>Transaction history</div>}
                            </Button>
                            </Col>
                            <Col xs={5}>
                                {historyType === true && <div>Account history</div>}
                                {historyType === false && <div>Transaction history</div>}
                            </Col>
                            <Col xs={3}>
                                <DropdownButton style={{"float":"right"}} menuAlign="right" variant="primary" title={data.last_symbol} onSelect={onSymbolChange}>
                                    {getSymbols}
                                </DropdownButton>
                            </Col>
                        </Row>
                    </Card.Header>
                    <Card.Body>
                        {historyType === false && 
                            <div>
                                <Card className="bg-secondary text-white">
                                    <Container>
                                        <Row> <Col>Date & Time</Col> <Col>Amount</Col> <Col>Currency</Col> <Col>Note</Col> </Row>
                                    </Container>
                                </Card>
                                <hr className="border-0"/>
                                {showHistory}
                            </div>}
                        {historyType === true &&
                            <Line 
                            data={getGraphHistory(data.last_symbol)}
                            options={{
                                responsive: true,
                                scales: {
                                    yAxes: [{
                                        ticks: {
                                            autoSkip: true,
                                            maxTicksLimit: 10,
                                            beginAtZero: true,
                                            fontColor: '#f7f7f7'
                                        },
                                        gridLines: {
                                            display: true,
                                            color: '#878787',
                                            borderDash: [5]
                                        }
                                    }],
                                    xAxes: [{
                                        ticks: {
                                            fontColor: '#f7f7f7'
                                        },
                                        gridLines: {
                                            display: true,
                                            color: '#878787',
                                            borderDash: [5]
                                        }
                                    }]
                                },
                                legend: { 
                                    position: 'top',
                                    labels: {
                                        fontColor: '#f7f7f7'
                                    }
                                }
                            }}
                            />
                        }
                    </Card.Body>

                </Card>
            </Container>
            </div>
        </div>
    )
}

export default History