import React from 'react';

import Container from 'react-bootstrap/Container'
import Row from 'react-bootstrap/Row';
import Col from 'react-bootstrap/Col';
import Card from 'react-bootstrap/Card'
import Listgroup from 'react-bootstrap/ListGroup';
import TopBar from '../../Components/TopBar'
import Button from 'react-bootstrap/esm/Button';
import Modal from 'react-bootstrap/Modal';
import { ArrowCounterclockwise } from 'react-bootstrap-icons'

import { useState, useEffect } from 'react';
import axios from 'axios';

function Home(props) {
    var data = props.location.data; //main data variable
    var acc_type = ["DEMO", "NORMAL", "ADMIN"];

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

    const [refresh, setRefresh] = useState(false);

    //list balances map
    const listBalance = data.balances.map((balance) =>
        <Listgroup.Item className="bg-dark text-light border-light"
                        key={balance.symbol} 
                        style={{"textAlign": "left"}}>
                <Row>
                    <Col>
                        {balance.symbol}:
                    </Col>
                    <Col>
                        {balance.balance}
                    </Col>
                </Row>
        </Listgroup.Item>
    );

    //update balance ajax
    const updateBalances = async(want_refresh) => {
            try {
                let response = await axios.get("http://"+data.ip+"/currencies/balances", {headers: {"Content-Type": "application/json", "Authorization": "Bearer "+data.token}});
                data.balances = response.data;
                if (want_refresh === true) setRefresh(!refresh);
            }
            catch(error){
                setError("Server is unavailable", true, "warning");
            }
        }

    //update on load (kinda does not work)
    useEffect(() => {
        updateBalances(false);
    });

    return (
        <div className="HomePage">
            {/*Error modal*/}
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
            {/*Header menu*/}
            <div className="HPHeader">
                <TopBar page_num={1} data={data}/>
            </div>
            {/*Main body*/}
            <div className="HPMainBody">
                <Container>
                    <Row> 
                        <Col>
                            <Card className="bg-dark text-light border border-light">
                                <Card.Header className="border-right-0 border-left-0 border-top-0 border border-light" 
                                             style={{'textAlign': 'center'}}
                                             as="h4"
                                >
                                    Account information
                                </Card.Header>
                                <Card.Body>
                                    <Card.Text> 
                                        <Listgroup variant="flush">
                                            <Listgroup.Item className="bg-dark border-light"><Row><Col>User:         </Col> <Col>{data.user_data.username}                </Col></Row></Listgroup.Item>
                                            <Listgroup.Item className="bg-dark border-light"><Row><Col>Account type: </Col> <Col>{acc_type[data.user_data.acc_type]}      </Col></Row></Listgroup.Item>
                                            <Listgroup.Item className="bg-dark border-light"><Row><Col>Last login:   </Col> <Col>{data.user_data.last_login}              </Col></Row></Listgroup.Item>
                                            <Listgroup.Item className="bg-dark border-light"><Row><Col>Created on:   </Col> <Col>{data.user_data.creation_time}           </Col></Row></Listgroup.Item>
                                            <hr/>
                                        </Listgroup>
                                    </Card.Text>
                                </Card.Body>
                            </Card>
                        </Col>
                        <Col md={4}>
                            <Card className="bg-dark border border-light">
                                <Card.Header className="border-right-0 border-left-0 border-top-0 border border-light"
                                             style={{'textAlign': 'center'}}
                                             as="h4"
                                >
                                    Balances
                                    <Button style={{"float":"right"}} onClick={() => updateBalances(true)}>
                                        <ArrowCounterclockwise/>
                                    </Button>
                                </Card.Header>
                                <Card.Body>
                                    <Card.Text>
                                        <Listgroup variant="flush">
                                            {listBalance}
                                            <hr className="bg-light"/>
                                        </Listgroup>
                                    </Card.Text>
                                </Card.Body>
                            </Card>
                        </Col>
                    </Row>
                </Container>
            </div>
        </div>
    )
}

export default Home