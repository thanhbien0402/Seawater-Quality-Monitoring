'use client'
import {
    Drawer,
    DrawerClose,
    DrawerContent,
    DrawerDescription,
    DrawerFooter,
    DrawerHeader,
    DrawerTitle,
    DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { BadgeAlert } from 'lucide-react'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { useState } from 'react'
import NotificationDeviceTable from '@/app/notificationDevice/page'
import SensorRangeTable from '@/app/sensorRanges/page'


export function NotificationDeviceDrawer({ deviceId }: { deviceId: string }) {

    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Drawer open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DrawerTrigger asChild>
                <Button variant="ghost">
                    <BadgeAlert className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent>
                <Tabs defaultValue="notification" className="w-full">
                    <TabsList className="grid w-full grid-cols-2">
                        <TabsTrigger value="notification">Notification List</TabsTrigger>
                        <TabsTrigger value="range">Sensor Range</TabsTrigger>
                    </TabsList>
                    <TabsContent value="notification">
                        <DrawerHeader>
                            <DrawerTitle>Notification List</DrawerTitle>
                            <DrawerDescription>
                                View your notifications below.
                            </DrawerDescription>
                        </DrawerHeader>
                        <NotificationDeviceTable deviceId={deviceId} />
                    </TabsContent>
                    <TabsContent value="range">
                        <DrawerHeader>
                            <DrawerTitle>Sensor Range</DrawerTitle>
                            <DrawerDescription>
                                All of the sensor range to trigger for notifications
                            </DrawerDescription>
                        </DrawerHeader>
                        <SensorRangeTable deviceId={deviceId} />
                    </TabsContent>
                </Tabs>
                <DrawerFooter className="pt-2">
                    <DrawerClose asChild>
                        <Button type="button" variant="default">
                            Close
                        </Button>
                    </DrawerClose>
                </DrawerFooter>
            </DrawerContent>
        </Drawer>
    )
}